package com.teamnova.server;

import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.RecvLogics.Login;
import com.teamnova.server.RecvLogics.SaveResultDB;
import com.teamnova.server.RecvLogics.SendCharacterInfo;
import com.teamnova.server.RecvLogics.SendChatMessage;
import com.teamnova.server.RecvLogics.SendGameState;
import com.teamnova.server.RecvLogics.SendItem;
import com.teamnova.server.RecvLogics.SendMonsterInfo;
import com.teamnova.server.RecvLogics.SendRoomInfo;
import com.teamnova.server.RecvLogics.SendSystemMessage;
import com.teamnova.server.RecvLogics.SendUserInfo;
import com.teamnova.server.object.AttackInfo;
import com.teamnova.server.object.ChatMessage;
import com.teamnova.server.object.CheckPassword;
import com.teamnova.server.object.GameEnd;
import com.teamnova.server.object.GameReadyInfo;
import com.teamnova.server.object.GameStartInfo;
import com.teamnova.server.object.GameState;
import com.teamnova.server.object.ItemBuy;
import com.teamnova.server.object.ItemUsing;
import com.teamnova.server.object.LeaveRoom;
import com.teamnova.server.object.LoginData;
import com.teamnova.server.object.MonsterData;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.PlayerData;
import com.teamnova.server.object.RefreshItem;
import com.teamnova.server.object.Revival;
import com.teamnova.server.object.RoomInfo;
import com.teamnova.server.object.TopRanking;
import com.teamnova.server.object.Type;

import java.util.Random;

import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelId;
import io.netty.channel.ChannelInboundHandlerAdapter;

import static com.teamnova.server.MainServer.HT_ConnectedAllUserInfo;
import static com.teamnova.server.MainServer.IDCode;
import static com.teamnova.server.MainServer.roomList;
import static java.lang.Thread.sleep;

class RecvHandler extends ChannelInboundHandlerAdapter {
    private SendCharacterInfo sendCharacterInfo = new SendCharacterInfo();
    private SendMonsterInfo sendMonsterInfo = new SendMonsterInfo();



    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {
        if(!ctx.channel().isWritable()){
            System.out.println("No Writable Channel");
            return;
        }

        if(msg instanceof Packet){ // instanceof : 형변환이 가능한지 판단하는 연산자
            Packet packet = (Packet) msg;
            byte type = 0;
            try {
                type = packet.packetTypeType();
            }catch (IndexOutOfBoundsException e){
                System.out.println("OutofBound!!! : " + type);
                return;
            }

            if(type == Type.LoginData){
                LoginData loginData = (LoginData)packet.packetType(new LoginData());
                System.out.println("\n\n*----|");
                System.out.println("*----| Login Attempt..");
                System.out.println("*----|\n\n");

                boolean multilogin = false;
                if (HT_ConnectedAllUserInfo.containsKey(ctx.channel().id())) {
                    if(HT_ConnectedAllUserInfo.get(ctx.channel().id()).IsLogin) {
                        System.out.println("*----| This ID is already Login");
                        multilogin = true;
                    }
                }

                if (!multilogin) {
                    int idcode = IDCode++;
                    Login lg = new Login(ctx, loginData,idcode);
                    // 로그인에 성공한 경우에만 스레드에 로그인한 유저 아이디를 저장해준다.
                    if (lg.GameLogin()) { // 로그인 성공
                        String Userid = loginData.ID(); // 생성된 스레드에 아이디값 저장
                        int[] Inventory = lg.GetInventory(Userid);
                        SocketInfo UserInfo = new SocketInfo(ctx.channel(), Userid, 0, Inventory[0], idcode, Inventory[1], Inventory[2], Inventory[3]); // 소켓에 대한 유저 정보를 저장할 클래스 생성, 로그인할때는 게임상태는 -1
                        UserInfo.IsLogin = true;
                        HT_ConnectedAllUserInfo.put(ctx.channel().id(), UserInfo);

                    }
                }
            }else {
                Broadcast(packet, ctx);
            }
        }else {
            ctx.fireChannelRead(msg); // 쓸데없는 데이터이므로 버린다.
        }

    }

    @Override
    public void channelReadComplete(ChannelHandlerContext ctx) throws Exception {
        super.channelReadComplete(ctx);
        //System.out.println("다읽었음");
        ctx.flush();
    }

    private void Broadcast(Packet packet, ChannelHandlerContext ctx) throws InterruptedException {
        SocketInfo Current_User_Data = HT_ConnectedAllUserInfo.get(ctx.channel().id());
        String UserID = Current_User_Data.UserID; // 현재 데이터를 보낸 소켓의 로그인 아이디값
        ChannelId channelId =  ctx.channel().id();
        int Current_User_GameState = Current_User_Data.GameState;
        byte type = packet.packetTypeType();

        if(type == Type.GameState) {
            GameState gameState = (GameState) packet.packetType(new GameState()); // 회원가입 데이터로 변환
            if (gameState.Code() != -1) {
                // 받은 코드로 회원 목록에서 업데이트
                HT_ConnectedAllUserInfo.get(ctx.channel().id()).GameState = gameState.Code();
                Current_User_GameState = Current_User_Data.GameState; // 현재 이 스레드에서도 업데이트

                System.out.println("\n*----| Receive Code :  " + Current_User_GameState);

                if (gameState.Code() == 0) {
                    System.out.println("\n\n*----|");
                    System.out.println("*----| [ " + UserID + " ] = Enter the Lobby");
                    System.out.println("*----|");

                    try {
                        new SendGameState(channelId, UserID); // 자신을 제외한 다른 유저에게 게임정보를 보낸다.
                        sleep(100);
                        new SendUserInfo(ctx, channelId); // 로비에 들어온 유저만 리스트를 갱신해 받는다.(자신만)
                        sleep(100);
                        SendRoomInfo sr = new SendRoomInfo();
                        byte[] bytes = sr.MakeRoomListData();
                        sr.SendRoomData(channelId, bytes); // 현재생성된 방리스트 보내는 부분.
                        sleep(100);
                        new SendItem().SendInventory(channelId); // 해당 아이디가 현재 가진, 돈, 포션 개수를 보내준다.
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }



                } else if (gameState.Code() == 3) { // 로그아웃
                    System.out.println("\n\n*----|");
                    System.out.println("*----| [ " + UserID + " ] = Connection Close");
                    System.out.println("*----|");
                    new SendUserInfo(ctx, channelId);
                    sleep(100);
                    new SendGameState(channelId, UserID);

                    HT_ConnectedAllUserInfo.remove(ctx.channel().id()); // 리스트에서 접속 종료한 클라 제거
                    return;

                } else { // 로비에 있거나 로그아웃 상태가 아닌경우, 즉 게임을 하거나 방에 참여한 경우
                    if (Current_User_GameState >= 100) {
                        new SendRoomInfo().Send(channelId, 2); // 방에 들어가 방정보를 갱신받기 위한 부분이다. 특히 참여 유저 정보
                        sleep(100);
                    }
                    new SendGameState(channelId, UserID); // 자신을 제외한 다른 유저에게 내가 방에 들어갔다는 게임정보를 보낸다.
                }
            }else if(gameState.Code() == -1){

                    return;
            }

        }

        // 상점에서 내 인벤토리 새로고침을 눌렀을때
        if(type == Type.RefreshItem){
            System.out.println("Item Refresh");
            new SendItem().SendInventory(channelId); // 해당 아이디가 현재 가진, 돈, 포션 개수를 보내준다.
        }

        // 아이템 구매버튼을 누른 경우
        // 현재 가진 금액과 비교해서 업데이트 해준다.
        if(type == Type.ItemBuy){
            ItemBuy itemBuy = (ItemBuy) packet.packetType(new ItemBuy());

            if(itemBuy.TotalPrice() != 0) { // 구매금액이 0이 아닌경우에만
                // 구매하려는 아이템의 총액이 가진 금액보다 큰경우 -> 구매불가
                if (itemBuy.TotalPrice() > HT_ConnectedAllUserInfo.get(channelId).Gold) {
                    System.out.println("Buy Failed");
                    new SendItem().SendBuyRequest(channelId, false);

                } else { // 그 외의 경우 -> 구매가능
                    System.out.println("Buy Successful");
                    HT_ConnectedAllUserInfo.get(channelId).Potion100 += itemBuy.Potion100();
                    HT_ConnectedAllUserInfo.get(channelId).Potion50 += itemBuy.Potion50();
                    HT_ConnectedAllUserInfo.get(channelId).Gold -= itemBuy.TotalPrice();
                    new SendItem().SendBuyRequest(channelId, true);
                    sleep(100);
                    new SaveResultDB().SaveItemToDB(channelId);
                    sleep(100);
                    new SendItem().SendInventory(channelId); // 해당 아이디가 현재 가진, 돈, 포션 개수를 보내준다.


                }
            }
        }

        if(type == Type.ItemUsing){
            ItemUsing itemUsing = (ItemUsing) packet.packetType(new ItemUsing());
            int itemtype = itemUsing.Type();
            int UsingUSer = itemUsing.UserID();
            System.out.println("ItemUsing!!! : " + itemtype + "%");
            if(itemtype == 100){ // 100% 회복 아이템을 사용한 경우
                for(int i=0; i<roomList.get(Current_User_GameState).RoomUser.size(); i++){
                    if(roomList.get(Current_User_GameState).RoomUser.get(i).UseridCode == UsingUSer){
                        if(HT_ConnectedAllUserInfo.get(roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey).Potion100 > 0) {
                            roomList.get(Current_User_GameState).RoomUser.get(i).PlayerHealth = 1000;
                            HT_ConnectedAllUserInfo.get(roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey).Potion100
                                    = --roomList.get(Current_User_GameState).RoomUser.get(i).Potion100;
                            new SendSystemMessage().SendSpecific("100% 회복포션을 사용하였습니다.", 4, roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey);
                            sendCharacterInfo.SendPlayerData_In_Playing(Current_User_GameState, channelId, roomList.get(Current_User_GameState).RoomUser.get(i).PlayerData());

                        }
                    }
                }
            }else {
                for(int i=0; i<roomList.get(Current_User_GameState).RoomUser.size(); i++){
                    if(roomList.get(Current_User_GameState).RoomUser.get(i).UseridCode == UsingUSer){
                        if(HT_ConnectedAllUserInfo.get(roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey).Potion50 > 0) {
                            roomList.get(Current_User_GameState).RoomUser.get(i).PlayerHealth += 500;
                            HT_ConnectedAllUserInfo.get(roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey).Potion50
                                    = --roomList.get(Current_User_GameState).RoomUser.get(i).Potion50;
                            new SendSystemMessage().SendSpecific("50% 회복포션을 사용하였습니다.", 4, roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey);

                            if (roomList.get(Current_User_GameState).RoomUser.get(i).PlayerHealth > 1000) {
                                roomList.get(Current_User_GameState).RoomUser.get(i).PlayerHealth = 1000;
                            }
                            sendCharacterInfo.SendPlayerData_In_Playing(Current_User_GameState, channelId, roomList.get(Current_User_GameState).RoomUser.get(i).PlayerData());

                        }
                    }
                }
            }
        }



        // System.out.println("여기도 진행되니?");
        // 클라이언트로부터 채팅 메세지를 받았을때
        if(type == Type.ChatMessage){
            ChatMessage chatMessage = (ChatMessage) packet.packetType(new ChatMessage()); // 회원가입 데이터로 변환
            System.out.println("\n*----| Chatting Message Sender : " + chatMessage.SenderID() + " | " + chatMessage.ChatMsg());
            new SendChatMessage().Send(chatMessage, channelId);
        }

        // 클라이언트로부터 탑10 랭킹을 데이터를 요청받았을때
        if(type == Type.TopRanking)
        {
            TopRanking topRanking = (TopRanking) packet.packetType(new TopRanking());
            if(topRanking.request() == true){
                new SaveResultDB().SendTopRanking(channelId);
            }
        }


        // 플레이어 데이터를 받는 경우, 이 경우는 게임 진행중일때만 받는다.
        if(type == Type.PlayerData){
            PlayerData playerData = (PlayerData) packet.packetType(new PlayerData());

            for(int i=0; i<roomList.get(Current_User_GameState).RoomUser.size(); i++){
                if(roomList.get(Current_User_GameState).RoomUser.get(i).UseridCode == playerData.playerName()){
                    //System.out.println("PlayerData Recv : " + playerData.playerName());
                    roomList.get(Current_User_GameState).RoomUser.get(i).UpdatePlayingUserData(playerData);
                    sendCharacterInfo.SendPlayerData_In_Playing(Current_User_GameState, channelId, roomList.get(Current_User_GameState).RoomUser.get(i).PlayerData());
                    break;
                }
            }
        }


        // 방정보 데이터를 받는 경우
        if(type == Type.RoomInfo){
            RoomInfo roomInfo = (RoomInfo) packet.packetType(new RoomInfo());

            System.out.println("\n*----| Received RoomNumber : " + roomInfo.RoomNumber());
            System.out.println("*----| My Current GameCode : " + Current_User_GameState);
            if(roomInfo.RoomNumber() == -1){ // 방을 생성한 경우
                new SendRoomInfo().MakingRoom(roomInfo, channelId, HT_ConnectedAllUserInfo.get(channelId).IDCode, UserID); // 만든방의 데이터를 본인이 받는다.
            }else { // 그외 방에 들어가는 경우, 만들어진 방의 데이터를 받는다.
                new SendRoomInfo().EnterRoom(roomInfo, HT_ConnectedAllUserInfo.get(channelId).IDCode, channelId, UserID);
            }
        }



        // 방에 참여해서 게임 준비와 관련된 데이터를 받는 경우
        if(type == Type.GameReadyInfo)
        {
            GameReadyInfo gameReadyInfo = (GameReadyInfo) packet.packetType(new GameReadyInfo());
            System.out.println("*----| Ready!!");
            // 룸정보의 데이터를 변경해주고,
            for(int i=0; i< roomList.get(gameReadyInfo.ReadyRoomnumber()).RoomUser.size(); i++){
                if(roomList.get(gameReadyInfo.ReadyRoomnumber()).RoomUser.get(i).UseridCode == gameReadyInfo.ReadyUserId()){
                    roomList.get(gameReadyInfo.ReadyRoomnumber()).RoomUser.get(i).isready = gameReadyInfo.ready();
                    break;
                }
            }
            // 뿌려준다.
            new SendRoomInfo().Send(channelId, 3);
        }


        // 비밀방입장, 패스워드 체크
        if(type == Type.CheckPassword)
        {
            CheckPassword checkPassword = (CheckPassword)packet.packetType(new CheckPassword());
            boolean result = false;
            if(roomList.get(checkPassword.roomnumber()).Password.equals(checkPassword.password())){
                result = true;
            }
            FlatBufferBuilder fb = new FlatBufferBuilder(8192);
            CheckPassword.startCheckPassword(fb);
            CheckPassword.addIscorrect(fb, result);
            int ch = CheckPassword.endCheckPassword(fb);
            int pk = Packet.createPacket(fb, Type.CheckPassword, ch);
            fb.finish(pk);
            byte[] bytes = fb.sizedByteArray();

            HT_ConnectedAllUserInfo.get(channelId).socket.writeAndFlush(bytes);

        }


        // 방을 나가는 데이터를 받은 경우
        if(type == Type.LeaveRoom){
                LeaveRoom leaveRoom = (LeaveRoom)packet.packetType(new LeaveRoom());
                System.out.println("\n\n*----| Leave Room");
                if(roomList.get(leaveRoom.LeaveRoomnumber()).RoomUser.size() == 1){ // 1인방 이거나 방에 혼자남은 경우
                    System.out.println("*----| " + leaveRoom.LeaveRoomnumber() + " Room Exploded!!");
                    System.out.println("*----|\n\n");
                    roomList.remove(leaveRoom.LeaveRoomnumber()); // 방 자체를 없애준다.

                }else {
                    // 그 외의 경우는 방에 있는 유저들의 리스트를 탐색해서 나가는 사람을 대조한 후 리스트에서 제거해준다.
                    for(int i=0; i< roomList.get(leaveRoom.LeaveRoomnumber()).RoomUser.size(); i++){
                        if(roomList.get(leaveRoom.LeaveRoomnumber()).RoomUser.get(i).UseridCode == leaveRoom.LeaveUserID()){
                            // 이 경우는 나가는 사람이 방장인 경우이다. 다음 번호에 있는 사람이 방장이 되도록 바꿔준다.
                            if(roomList.get(leaveRoom.LeaveRoomnumber()).RoomOwner == leaveRoom.LeaveUserID()){
                                System.out.println("\n*----| Ooopps! Leave RoomOwner!");
                                roomList.get(leaveRoom.LeaveRoomnumber()).RoomUser.remove(i);
                                roomList.get(leaveRoom.LeaveRoomnumber()).RoomOwner = roomList.get(leaveRoom.LeaveRoomnumber()).RoomUser.get(i).UseridCode;
                                // 이 경우는 방장이 아닌 사람이 나가는 경우이다. 그냥 리스트에서 제거해준다.
                            }else {
                                System.out.println("\n*----| Please Don't Leave :(");
                                roomList.get(leaveRoom.LeaveRoomnumber()).RoomUser.remove(i);
                            }
                            break;
                        }
                    }
                    // 현재 방에 참여하고 있는 유저들의 숫자를 업데이트 해준다.
                    roomList.get(leaveRoom.LeaveRoomnumber()).CurrentUserNumber =  roomList.get(leaveRoom.LeaveRoomnumber()).RoomUser.size();
                    // 방 정보를 방에 참여하고 있는 사람들에게 보낸다.
                    new SendRoomInfo().Send(channelId, 1);
                }
                //sleep(100);
                // 이 부분은 로비에 있는 사람들의 룸 리스트를 업데이트 해주는 부분이다.
                new SendRoomInfo().UpdateRoomList();
        }

        //TODO: 이제 게임시작 버튼을 누르고부터 데이터 처리
        //TODO: 게임플레이는 다른 방식을 생각해보자.

        // 게임 시작 버튼을 눌렀을때 게임 시작을 시켜줘야 한다.
        if(type == Type.GameStartInfo){
            GameStartInfo gameStartInfo = (GameStartInfo)packet.packetType(new GameStartInfo());

            System.out.println("\n\n*----|");
            System.out.println("*----| GameStart");
            int startGameRoomnumber = gameStartInfo.StartGameRoomnumber();
            //  이 메세지를 보내는 경우는 방장이 게임 시작버튼을 눌렀을때 뿐이다.
            if(!gameStartInfo.isstart()){
                System.out.println("*----| GameStart Counting!! in Room");
                // 아직 게임 시작이 아니란 의미, 스레드를 실행시켜 카운트를 해준다.
                // 이 부분이 true 가 되는 경우는 openlevel을 진행하기 바로 직전?
                roomList.get(startGameRoomnumber).First_Count_In_Room = true;
                roomList.get(startGameRoomnumber).Is_This_Room_Game_Start = true;
                new GamePlayThread(startGameRoomnumber).start();
                new SendRoomInfo().UpdateRoomList();

            }else {
                // 이제 게임 시작버튼을 누르고, 카운트가 완료되어서 PlayLevel로 맵이동을 한 경우이다.
                // isstart가 모두 true가 되었으므로, 방에 참여한 플레이어 모두 게임을 시작한 것이다.
                // 이제 로딩이 완료되면 게임이 시작된다.
                if(gameStartInfo.isloadingcomplete()){

                    for(int i=0; i< roomList.get(startGameRoomnumber).RoomUser.size(); i++){
                        if( roomList.get(startGameRoomnumber).RoomUser.get(i).ServerLoginKey == channelId){
                            roomList.get(startGameRoomnumber).RoomUser.get(i).isloadingcomplete = true;
                            System.out.println("*----| Loading Complete!! : " + roomList.get(startGameRoomnumber).RoomUser.get(i).Userid);
                            break;
                        }
                    }

                    for(int i=0; i< roomList.get(startGameRoomnumber).RoomUser.size(); i++){
                        if(roomList.get(startGameRoomnumber).RoomUser.get(i).isloadingcomplete){
                            if(i == roomList.get(startGameRoomnumber).RoomUser.size()-1) {
                                System.out.println("*----| AllUser Loading Complete, GameCount Start In Game!!");
                                roomList.get(startGameRoomnumber).Second_Count_In_Game = true;
                            }
                        }else {
                            break;
                        }
                    }
                }
            }
        }

        if(type == Type.SpawnOK) // 몬스터를 스폰했다라는 메세지
        {
            System.out.println("SpawnOK");
            // 이 메세지를 받았다면, 이 메세지를 전송한 클라이언트는 몬스터를 스폰했다는 의미이므로
            // 방리스트에서 해당 유저의 데이터를 업데이트해준다.
            for(int i=0; i<roomList.get(Current_User_GameState).RoomUser.size(); i++){
                if(roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey == channelId){
                    roomList.get(Current_User_GameState).RoomUser.get(i).IsMonsterSpawn = true;
                }
            }
        }

        if(type == Type.MoveOK)
        {
            System.out.println("MoveOK");
            roomList.get(Current_User_GameState).MonsterSpawnOK = true;
        }


        if(type == Type.MonsterData){
            MonsterData monsterData = (MonsterData)packet.packetType(new MonsterData());
            // 몬스터 정보를 받으면, 즉 이클라이언트가 방장인 것이고, 이 정보를 업데이트 해줘야 된다.
            // 그리고 방장을 제외한 다른 유저들에게 정보를 뿌려줘야 한다. 그래야 몬스터 정보가 동기화됨
            // 받은 몬스터 정보를 이름으로 구별해서 데이터를 업데이트해주자.

            // 만약 받은 몬스터의 정보가 A몬스터에 대한 정보라면, A몬스터에 대한 정보를 서버에 있는 몬스터리스트에 업데이트해주고,
            // A몬스터의 정보만 방장 포함해서(방장은 몬스터의 체력정보를 얻어야 하므로) 다른 클라에게 전달해준다.
            for(int i=0; i<roomList.get(Current_User_GameState).MonsterList.size(); i++){
                if(roomList.get(Current_User_GameState).MonsterList.get(i).name == monsterData.playerName()){
                    roomList.get(Current_User_GameState).MonsterList.get(i).UpdateMonsterData(monsterData);
                    sendMonsterInfo.Send(Current_User_GameState, i);
                }
            }
            //System.out.println("몬스터 수 : " + roomList.get(HT_ConnectedAllUserInfo.get(Userid).GameState).MonsterList.size());
        }

        if(type == Type.Revival){
            Revival revival = (Revival)packet.packetType(new Revival());

            // 부활하겠다라는 메세지를 클라이언트에서 받는 경우
            // 누가 보낸 메세지인지 확인하기 위해 탐색한다.\
            // 이 경우는 게임플레이할떄만 받을수있음
            for(int i=0; i<roomList.get(Current_User_GameState).RoomUser.size(); i++){
                if(roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey == channelId){
                    if(revival.IsPerfect()){ // 100% 부활인 경우
                        roomList.get(Current_User_GameState).RoomUser.get(i).PlayerHealth = 1000; // 캐릭터 최대 체력 1000
                        roomList.get(Current_User_GameState).RoomUser.get(i).IsPlayerAlive = true; // 캐릭터 최대 체력 1000
                    }else { // 30% 부활인 경우
                        roomList.get(Current_User_GameState).RoomUser.get(i).PlayerHealth = 300;
                        roomList.get(Current_User_GameState).RoomUser.get(i).IsPlayerAlive = true;
                    }
                }
            }
        }


        if(type == Type.GameEnd){
            System.out.println("GameEnd Packet Recv : " + HT_ConnectedAllUserInfo.get(channelId).UserID);
            // 게임종료 메세지를 받았다는 처리를 하기 위함
           // int EndUserNum = 0;
            //GameEnd gameEnd = (GameEnd)packet.packetType(new GameEnd());
            for(int i=0; i<roomList.get(Current_User_GameState).RoomUser.size(); i++){
                if(roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey == channelId){
                    System.out.println(roomList.get(Current_User_GameState).RoomUser.get(i).Userid + " = End!!");
                    roomList.get(Current_User_GameState).RoomUser.get(i).GameEndOK = true;
                }
             /*   if(roomList.get(Current_User_GameState).RoomUser.get(i).GameEndOK){
                    EndUserNum++;
                }*/
            }
           /* System.out.println("Game End User Num : " + EndUserNum);
            if(EndUserNum == roomList.get(Current_User_GameState).RoomUser.size()){
                System.out.println("All User Game End");
                roomList.get(Current_User_GameState).GameEnd = true;
            }*/
        }


        if(type == Type.AttackInfo){
            AttackInfo attackInfo = (AttackInfo)packet.packetType(new AttackInfo());

            System.out.println("*----| AttackInfo");
            System.out.println("*----| Attacker [" + attackInfo.AttackerID() + "], Target [" + attackInfo.TargetID() + "], AttackType [" + attackInfo.AttackType() + "]");
            System.out.println("*----|\n");
            // 공격자와 피공격자는 숫자값이기 때문에 누가 캐릭터이고 몬스터인지 구분할수없다.
            // 캐릭터와 몬스터를 구분하는 기준은 어택 타입
            // 몬스터가 공격자인 경우는 어택 타입이 10이상
            // 캐릭터가 공격자인 경우는 어택 타입이 1~10
            // 예외적으로 타워의 코드는 999, 이는 플레이 회원수가 999를 넘지않는다는 가정하에 작성한것
            if(attackInfo.AttackType() >= 10){ // 몬스터의 공격인 경우
                if(attackInfo.TargetID() == 999){ // 타워의 아이디 코드 999 - 타워가 몬스터에게 공격받은 경우이다.
                    switch (attackInfo.AttackType()){
                        case 10: // 일반몬스터 공격
                            roomList.get(Current_User_GameState).FountainHealth -= 50;

                            break;
                        case 11: // 레어몬스터 공격
                            roomList.get(Current_User_GameState).FountainHealth -= 100;

                            break;
                        case 12: // 보스몬스터 공격
                            roomList.get(Current_User_GameState).FountainHealth -= 200;

                            break;
                    }
                    System.out.println("*----| Tower Damaged!! HP :  " + roomList.get(Current_User_GameState).FountainHealth);
                    if(roomList.get(Current_User_GameState).GameEnd && roomList.get(Current_User_GameState).FountainHealth < 0){
                        // 게임이 끝났는데, 계속 타워의 체력이 감소되는 경우, 아직 방장클라의 게임이 종료가 안되었다는 의미
                        // 방장클라이언트를 종료시켜준다.
                        System.out.println("Room Owner Client End!!!");
                        FlatBufferBuilder fb = new FlatBufferBuilder(8192);
                        int ge = GameEnd.createGameEnd(fb, false);
                        int pk = Packet.createPacket(fb, Type.GameEnd, ge);
                        fb.finish(pk);
                        for(int i=0; i< roomList.get(Current_User_GameState).RoomUser.size(); i++){
                            if(roomList.get(Current_User_GameState).RoomUser.get(i).UseridCode == roomList.get(Current_User_GameState).RoomOwner){
                                HT_ConnectedAllUserInfo.get(roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey).socket.writeAndFlush(fb.sizedByteArray());

                            }
                        }


                    }
                    new SendRoomInfo().SendTowerHealth(Current_User_GameState);
                }else { // 캐릭터가 몬스터에게 공격받은 경우

                    for (int i = 0; i < roomList.get(Current_User_GameState).RoomUser.size(); i++) { // 어차피 같이 플레이하는 회원은 많아봐야 4명이므로, 그냥 배열리스트 탐색으로 하자.
                        if (roomList.get(Current_User_GameState).RoomUser.get(i).UseridCode == attackInfo.TargetID()) {
                            switch (attackInfo.AttackType()) {
                                case 10: // 일반몬스터 공격
                                    roomList.get(Current_User_GameState).RoomUser.get(i).PlayerHealth -= 50;
                                    roomList.get(Current_User_GameState).RoomUser.get(i).TotalReceivedDamage += 50;
                                    System.out.println("*----| Normal Monster Attack!");

                                    break;
                                case 11: // 레어몬스터 공격
                                    roomList.get(Current_User_GameState).RoomUser.get(i).PlayerHealth -= 100;
                                    roomList.get(Current_User_GameState).RoomUser.get(i).TotalReceivedDamage += 100;
                                    System.out.println("*----| Rare Monster Attack!");

                                    break;
                                case 12: // 보스몬스터 공격
                                    roomList.get(Current_User_GameState).RoomUser.get(i).PlayerHealth -= 200;
                                    roomList.get(Current_User_GameState).RoomUser.get(i).TotalReceivedDamage += 200;
                                    System.out.println("*----| Boss Monster Attack!!");

                                    break;
                            }
                            // 죽은횟수 증가시키는 부분
                            if (roomList.get(Current_User_GameState).RoomUser.get(i).PlayerHealth <= 0) {
                                roomList.get(Current_User_GameState).RoomUser.get(i).IsPlayerAlive = false; // 죽었으므로, 죽은 상태로 변경
                                roomList.get(Current_User_GameState).RoomUser.get(i).TotalDiedValue++;
                            }
                            System.out.println("*----| [ " + roomList.get(Current_User_GameState).RoomUser.get(i).Userid + " ]'s HP : " + roomList.get(Current_User_GameState).RoomUser.get(i).PlayerHealth);
                            System.out.println("*----| [ " + roomList.get(Current_User_GameState).RoomUser.get(i).Userid + " ]'s Dmg Point : " + roomList.get(Current_User_GameState).RoomUser.get(i).TotalReceivedDamage + "\n\n");

                        }
                    }
                    // 몬스터의 공격을 캐릭터가 받은 경우에만 체력 데이터를 보내주면 된다.
                    // 그리고 이 체력데이터는 공격받은 당사자만 받으면 된다.
                    // 왜냐면 어차피 캐릭터 데이터를 뿌려주는 메소드가 따로 있기 때문에,
                    // 하지만 A클라에서 보낸 A캐릭터 데이터는 A가 받지 않기 떄문에, A는 체력데이터만 갱신하기 위해 데이터를 전달받아야 한다.
                    //sendCharacterInfo.SendHealth(Current_User_GameState, attackInfo.TargetID());
                }



            }else { // 캐릭터의 공격인 경우
                int MonsterKillID = 0;
                int types = 0;
                for(int i=0; i<roomList.get(Current_User_GameState).MonsterList.size(); i++){
                    if(roomList.get(Current_User_GameState).MonsterList.get(i).name == attackInfo.TargetID()){
                        switch (attackInfo.AttackType()){
                            case 1: //일반공격
                                roomList.get(Current_User_GameState).MonsterList.get(i).health -= 50;
                                break;
                            case 2: // 스킬1
                                roomList.get(Current_User_GameState).MonsterList.get(i).health -= 100;
                                break;
                            case 3: // 스킬2
                                roomList.get(Current_User_GameState).MonsterList.get(i).health -= 200;
                                break;
                        }

                        if(roomList.get(Current_User_GameState).MonsterList.get(i).health <= 0 && roomList.get(Current_User_GameState).MonsterList.get(i).isAlive){
                            MonsterKillID = attackInfo.AttackerID();
                            types = roomList.get(Current_User_GameState).MonsterList.get(i).MonsterType;
                            roomList.get(Current_User_GameState).MonsterList.get(i).isAlive = false;
                        }
                        System.out.println("*----|");
                        System.out.println("*----| Monster [" + roomList.get(Current_User_GameState).MonsterList.get(i).name + "]'s HP : " + roomList.get(Current_User_GameState).MonsterList.get(i).health +
                                "(IsAlive? : " + roomList.get(Current_User_GameState).MonsterList.get(i).isAlive + ")");
                    }
                }

                // 몬스터에게 준 대미지 포인트 올리는 부분
                for(int i=0; i<roomList.get(Current_User_GameState).RoomUser.size(); i++) {
                    // 공격자인 경우에만 포인트를 올려준다. 이부분이 실행되었다는 것은 이미 공격에 성공해서 몬스터의 체력이 감소되었단 의미
                    if(roomList.get(Current_User_GameState).RoomUser.get(i).UseridCode == attackInfo.AttackerID()){
                        switch (attackInfo.AttackType()){
                            case 1: //일반공격
                                roomList.get(Current_User_GameState).RoomUser.get(i).TotalDamageToMonster += 50;
                                break;
                            case 2: // 스킬1
                                roomList.get(Current_User_GameState).RoomUser.get(i).TotalDamageToMonster += 100;
                                break;
                            case 3: // 스킬2
                                roomList.get(Current_User_GameState).RoomUser.get(i).TotalDamageToMonster += 200;
                                break;
                        }

                        System.out.println("*----| [ " + roomList.get(Current_User_GameState).RoomUser.get(i).Userid + " ]'s Attack Point : " + roomList.get(Current_User_GameState).RoomUser.get(i).TotalDamageToMonster + "\n\n");

                        if(roomList.get(Current_User_GameState).RoomUser.get(i).UseridCode == MonsterKillID){
                            Random rnad = new Random();


                            if(types == 1){ // 노멀
                                roomList.get(Current_User_GameState).RoomUser.get(i).KilledMonsterValue++;
                                System.out.println("*----| Killed Monster!! [ " + roomList.get(Current_User_GameState).RoomUser.get(i).Userid + " ]' Nomal Monster Kill num : " + roomList.get(Current_User_GameState).RoomUser.get(i).KilledMonsterValue + "\n\n");
                                roomList.get(Current_User_GameState).RoomUser.get(i).Gold+= 50;
                                new SendSystemMessage().SendSpecific("노멀몬스터를 잡아 50골드를 획득했습니다.", 4, roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey);


                            }else if(types == 2){ // 레어
                                roomList.get(Current_User_GameState).RoomUser.get(i).KilledMonsterValue_Rare++;
                                System.out.println("*----| Killed Monster!! [ " + roomList.get(Current_User_GameState).RoomUser.get(i).Userid + " ]' Rare Monster Kill num : " + roomList.get(Current_User_GameState).RoomUser.get(i).KilledMonsterValue_Rare + "\n\n");
                                roomList.get(Current_User_GameState).RoomUser.get(i).Gold+= 150;
                                new SendSystemMessage().SendSpecific("레어몬스터를 잡아 150골드를 획득했습니다.", 4, roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey);

                                if(rnad.nextInt(10) > 5){ // 50% 확률
                                    roomList.get(Current_User_GameState).RoomUser.get(i).Potion50++;
                                    new SendSystemMessage().SendSpecific("레어몬스터에게 50%체력회복포션을 얻었습니다.", 4, roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey);
                                }

                            }else if(types == 3){ // 보스
                                roomList.get(Current_User_GameState).RoomUser.get(i).KilledMonsterValue_Boss++;
                                System.out.println("*----| Killed Monster!! [ " + roomList.get(Current_User_GameState).RoomUser.get(i).Userid + " ]' Boss Monster Kill num : " + roomList.get(Current_User_GameState).RoomUser.get(i).KilledMonsterValue_Boss + "\n\n");
                                roomList.get(Current_User_GameState).RoomUser.get(i).Gold+= 500;
                                new SendSystemMessage().SendSpecific("보스몬스터를 잡아 500골드를 획득했습니다.", 4, roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey);

                                if(rnad.nextInt(10) > 7){ // 50% 확률
                                    roomList.get(Current_User_GameState).RoomUser.get(i).Potion100++;
                                    new SendSystemMessage().SendSpecific("보스몬스터에게 100%체력회복포션을 얻었습니다.", 4, roomList.get(Current_User_GameState).RoomUser.get(i).ServerLoginKey);
                                }
                            }
                            if(HT_ConnectedAllUserInfo.get(channelId).IDCode == roomList.get(Current_User_GameState).RoomUser.get(i).UseridCode) {
                                HT_ConnectedAllUserInfo.get(channelId).Potion100 = roomList.get(Current_User_GameState).RoomUser.get(i).Potion100;
                                HT_ConnectedAllUserInfo.get(channelId).Potion50 = roomList.get(Current_User_GameState).RoomUser.get(i).Potion50;
                                HT_ConnectedAllUserInfo.get(channelId).Gold = roomList.get(Current_User_GameState).RoomUser.get(i).Gold;
                            }


                        }
                    }
                }

            }

        }
    }
}
