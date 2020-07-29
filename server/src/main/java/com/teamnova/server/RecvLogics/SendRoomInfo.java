package com.teamnova.server.RecvLogics;


import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.GameRoomInfo;
import com.teamnova.server.MainServer;
import com.teamnova.server.RoomUserInfoObj;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.RoomInfo;
import com.teamnova.server.object.RoomList;
import com.teamnova.server.object.RoomUserInfo;
import com.teamnova.server.object.Type;

import io.netty.channel.ChannelId;

import static com.teamnova.server.MainServer.HT_ConnectedAllUserInfo;
import static com.teamnova.server.MainServer.roomList;

public class SendRoomInfo {

    public SendRoomInfo() {

    }

    public void SendTowerHealth(int roomnumber) {
        FlatBufferBuilder fb = new FlatBufferBuilder(8192);
        RoomInfo.startRoomInfo(fb);
        RoomInfo.addFountainHealth(fb, roomList.get(roomnumber).FountainHealth);
        int ri = RoomInfo.endRoomInfo(fb);
        int pk = Packet.createPacket(fb, Type.RoomInfo, ri);
        fb.finish(pk);
        byte[] bytes = fb.sizedByteArray();

        for (int i = 0; i < roomList.get(roomnumber).RoomUser.size(); i++) {

            HT_ConnectedAllUserInfo.get(roomList.get(roomnumber).RoomUser.get(i).ServerLoginKey).socket.writeAndFlush(bytes);
        }
    }

    public void MakingRoom(RoomInfo msg, ChannelId userid, int IDCode, String userID) {
        // 방을 생성하는 경우, 클라에서 -1로 생성된 방번호를 받는 경우이다. 서버에서 중복되지 않는 번호로 다시 방번호를 보내줘야 한다.
        int rn = MainServer.RoomNumber++;
        System.out.println("===================== Create Room =================");
        System.out.println("= Room Number : " + rn);
        // 방이 생성되고 생성된 방 정보를 서버에 저장하는 부분
        GameRoomInfo gri = new GameRoomInfo();
        gri.RoomName = msg.RoomName();
        gri.RoomOwner = msg.RoomOwner();
        gri.RoomUser.add(new RoomUserInfoObj(userID, IDCode, false, HT_ConnectedAllUserInfo.get(userid).MaxPoint, userid,  HT_ConnectedAllUserInfo.get(userid).Potion100, HT_ConnectedAllUserInfo.get(userid).Potion50, HT_ConnectedAllUserInfo.get(userid).Gold));
        gri.RoomNumber = rn;
        gri.MaxRoomUserNumber = msg.MaxUserNumber();
        gri.CurrentUserNumber = gri.RoomUser.size();
        gri.Password = msg.Password();
        gri.IsThisRoomLock = msg.IsThisRoomLock();
        System.out.println("= RoomInfo \n" + gri.RoomOwner + "/" + gri.RoomNumber + "/" + gri.MaxRoomUserNumber + "/" + gri.RoomName + " /password :  " + gri.Password + " /Lock? : " + gri.IsThisRoomLock);
        System.out.println("============================================\n");
        roomList.put(rn, gri); // 룸리스트에 생성된 방정보를 추가해준다.

        FlatBufferBuilder fb = new FlatBufferBuilder(8192);
        int[] userinfo = new int[roomList.get(rn).RoomUser.size()];
        for (int i = 0; i < roomList.get(rn).RoomUser.size(); i++) {
            userinfo[i] = RoomUserInfo.createRoomUserInfo(fb,
                    fb.createString(roomList.get(rn).RoomUser.get(i).Userid),
                    roomList.get(rn).RoomUser.get(i).UseridCode,
                    roomList.get(rn).RoomUser.get(i).isready,
                    roomList.get(rn).RoomUser.get(i).MaxPoint);
        }
        int UL = RoomInfo.createUserListVector(fb, userinfo);
        int name = fb.createString(gri.RoomName);
        int pw = fb.createString(gri.Password);
        RoomInfo.startRoomInfo(fb);
        RoomInfo.addRoomNumber(fb, gri.RoomNumber);
        RoomInfo.addCurrentUserNumber(fb, gri.CurrentUserNumber);
        RoomInfo.addRoomName(fb, name);
        RoomInfo.addMaxUserNumber(fb, gri.MaxRoomUserNumber);
        RoomInfo.addRoomOwner(fb, gri.RoomOwner);
        RoomInfo.addPassword(fb, pw);
        RoomInfo.addIsThisRoomLock(fb, gri.IsThisRoomLock);
        RoomInfo.addUserList(fb, UL);
        int ri = RoomInfo.endRoomInfo(fb);
        int pk = Packet.createPacket(fb, Type.RoomInfo, ri);
        fb.finish(pk);
        byte[] bytes = fb.sizedByteArray();

        System.out.println("RoomInfo -> SendSelf (Roominfo)");
        // 나 자신에겐 방정보만 보낸다. 다른방 리스트는 필요없다.
        HT_ConnectedAllUserInfo.get(userid).socket.writeAndFlush(bytes);

        //====================
        // 이 부분은 내가 방을 만들었으니 다른 유저들도 방이 만들어졌다는것을 알리기 위한 부분이다.
        // 생성된 방 리스트에 대한 정보를 나를 제외하고, 다른 유저들에게 뿌려준다.

        byte[] SendRoomList = MakeRoomListData();
        for (ChannelId key : HT_ConnectedAllUserInfo.keySet()) {
            if (userid != key) {
                System.out.println("RoomList -> Except ME (Roomlist)");
                SendRoomData(key,SendRoomList);
            }
        }
    }


    public void Send (ChannelId userid, int code){
        int current_room_number = HT_ConnectedAllUserInfo.get(userid).GameState;

        FlatBufferBuilder fb = new FlatBufferBuilder(8192);
        int[] userinfo = new int[roomList.get(current_room_number).RoomUser.size()];
        for (int i = 0; i < roomList.get(current_room_number).RoomUser.size(); i++) {
            int id =  fb.createString(roomList.get(current_room_number).RoomUser.get(i).Userid);
            RoomUserInfo.startRoomUserInfo(fb);
            RoomUserInfo.addUserid(fb, id);
            RoomUserInfo.addUseridCode(fb, roomList.get(current_room_number).RoomUser.get(i).UseridCode);
            RoomUserInfo.addMaxPoint(fb, roomList.get(current_room_number).RoomUser.get(i).MaxPoint);
            RoomUserInfo.addIsready(fb, roomList.get(current_room_number).RoomUser.get(i).isready);
            userinfo[i] = RoomUserInfo.endRoomUserInfo(fb);
        }

        int UL = RoomInfo.createUserListVector(fb, userinfo);
        int rn = fb.createString(roomList.get(current_room_number).RoomName);
        int pw = fb.createString(roomList.get(current_room_number).Password);
        RoomInfo.startRoomInfo(fb);
        RoomInfo.addRoomNumber(fb, roomList.get(current_room_number).RoomNumber);
        RoomInfo.addCurrentUserNumber(fb, roomList.get(current_room_number).CurrentUserNumber);
        RoomInfo.addRoomName(fb, rn);
        RoomInfo.addMaxUserNumber(fb, roomList.get(current_room_number).MaxRoomUserNumber);
        RoomInfo.addRoomOwner(fb, roomList.get(current_room_number).RoomOwner);
        RoomInfo.addPassword(fb, pw);
        RoomInfo.addUserList(fb, UL);
        int ri = RoomInfo.endRoomInfo(fb);
        int pk = Packet.createPacket(fb, Type.RoomInfo, ri);
        fb.finish(pk);
        byte[] bytes = fb.sizedByteArray();

        if (code == 1) { // 방에서 나가는 경우
            System.out.println(HT_ConnectedAllUserInfo.get(userid).UserID + " Leave Room");
            for (int i = 0; i < roomList.get(current_room_number).RoomUser.size(); i++) {
                ChannelId receiverID = roomList.get(current_room_number).RoomUser.get(i).ServerLoginKey;
                System.out.println("RoomInfo -> AllUser in Room(Roominfo)");
                SendRoomData(receiverID, bytes);
            }
        }else if(code == 2){ // 방생성후 유저 정보를 받기 위한 부분 호출한 유저에게만 데이터를 보낸다.
            SendRoomData(userid, bytes);
        }else if(code == 3){
            System.out.println("Game Ready!!! : " + HT_ConnectedAllUserInfo.get(userid).UserID);
            for(int i=0; i<roomList.get(current_room_number).RoomUser.size(); i++) {
                ChannelId receiverID = roomList.get(current_room_number).RoomUser.get(i).ServerLoginKey;
                System.out.println("Ready info Send(Roominfo) : " + receiverID);
                SendRoomData(receiverID, bytes);
            }
        }
    }





    public void SendRoomData(ChannelId userid, byte[] bytes) {
        // 현재 생성되어 있는 방리스트를 보내주는 부분. 특정 유저에게만 보내주는 부분이다.
        // 예를들어서 다른 상태에 있다가 로비에 들어온 경우, 현재 생성되어 있던 방정보를 갱신하기 위해 데이터를 받아야한다.
        //byte[] bytes1 = MakeRoomListData();
        System.out.println("[ Send RoomData ] -> [ " + HT_ConnectedAllUserInfo.get(userid).UserID + " ]");
        HT_ConnectedAllUserInfo.get(userid).socket.writeAndFlush(bytes);
    }

    public byte[] MakeRoomListData() {
        // 룸리스트를 스트림에 넣을 바이트배열로 만들어주는 메소드
        FlatBufferBuilder fb2 = new FlatBufferBuilder(8192);
        int i = 0;
        int[] RI = new int[roomList.size()];
        System.out.println("Current Maked Room Number : " + roomList.size());
        for (Integer key : roomList.keySet()) {
            int roomname = fb2.createString(roomList.get(key).RoomName);
            int pw = fb2.createString(roomList.get(key).Password);
            RoomInfo.startRoomInfo(fb2); // 현재 생성되어 있는 방정보를 담는다.
            RoomInfo.addRoomNumber(fb2, roomList.get(key).RoomNumber);
            RoomInfo.addCurrentUserNumber(fb2, roomList.get(key).CurrentUserNumber);
            RoomInfo.addRoomName(fb2, roomname);
            RoomInfo.addMaxUserNumber(fb2, roomList.get(key).MaxRoomUserNumber);
            RoomInfo.addIsThisRoomPlayingGame(fb2, roomList.get(key).Is_This_Room_Game_Start);
            RoomInfo.addIsThisRoomLock(fb2, roomList.get(key).IsThisRoomLock);
            RoomInfo.addPassword(fb2, pw);
            RoomInfo.addRoomOwner(fb2, roomList.get(key).RoomOwner);
            RI[i] = RoomInfo.endRoomInfo(fb2);
            i++;
        }
        int RL = RoomList.createRoomListVector(fb2, RI);
        RoomList.startRoomList(fb2);
        RoomList.addRoomList(fb2, RL);
        int rList = RoomList.endRoomList(fb2);
        int pk2 = Packet.createPacket(fb2, Type.RoomList, rList);
        fb2.finish(pk2);
        return  fb2.sizedByteArray();
    }

    public void EnterRoom(RoomInfo msg, int useridCode, ChannelId channelId, String userID) {
    // 방에 입장하는 경우, 처리하는 메소드
        if (HT_ConnectedAllUserInfo.get(channelId).GameState == 0
                && msg.RoomNumber() >= 100
                && roomList.get(msg.RoomNumber()).RoomOwner != useridCode) {
            System.out.println("[ Enter Other Player's Room ] -> [ " + msg.RoomNumber() + " ]");
            int current_room_number = msg.RoomNumber();
            roomList.get(current_room_number).RoomUser.add(new RoomUserInfoObj(userID, useridCode,false, HT_ConnectedAllUserInfo.get(channelId).MaxPoint, channelId,
                    HT_ConnectedAllUserInfo.get(channelId).Potion100, HT_ConnectedAllUserInfo.get(channelId).Potion50, HT_ConnectedAllUserInfo.get(channelId).Gold)); // 방에 참여했기 때문에 유저목록 업데이트
            roomList.get(current_room_number).CurrentUserNumber = roomList.get(current_room_number).RoomUser.size();

            FlatBufferBuilder fb = new FlatBufferBuilder(8192);

            int[] rui = new int[roomList.get(current_room_number).RoomUser.size()];
            for (int i = 0; i < roomList.get(current_room_number).RoomUser.size(); i++) {
                int id = fb.createString(roomList.get(current_room_number).RoomUser.get(i).Userid);
                RoomUserInfo.startRoomUserInfo(fb);
                RoomUserInfo.addUserid(fb, id);
                RoomUserInfo.addUseridCode(fb, roomList.get(current_room_number).RoomUser.get(i).UseridCode);
                RoomUserInfo.addMaxPoint(fb, roomList.get(current_room_number).RoomUser.get(i).MaxPoint);
                RoomUserInfo.addIsready(fb, roomList.get(current_room_number).RoomUser.get(i).isready);
                rui[i] = RoomUserInfo.endRoomUserInfo(fb);
            }
            int UL = RoomInfo.createUserListVector(fb, rui);
            int rn = fb.createString(roomList.get(current_room_number).RoomName);
            int pw = fb.createString(roomList.get(current_room_number).Password);
            RoomInfo.startRoomInfo(fb);
            RoomInfo.addRoomNumber(fb, roomList.get(current_room_number).RoomNumber);
            RoomInfo.addCurrentUserNumber(fb, roomList.get(current_room_number).CurrentUserNumber);
            RoomInfo.addRoomName(fb, rn);
            RoomInfo.addMaxUserNumber(fb, roomList.get(current_room_number).MaxRoomUserNumber);
            RoomInfo.addRoomOwner(fb, roomList.get(current_room_number).RoomOwner );
            RoomInfo.addIsThisRoomLock(fb, roomList.get(current_room_number).Is_This_Room_Game_Start);
            RoomInfo.addIsThisRoomLock(fb, roomList.get(current_room_number).IsThisRoomLock);
            RoomInfo.addPassword(fb, pw);
            RoomInfo.addUserList(fb, UL);
            int ri = RoomInfo.endRoomInfo(fb);
            int pk = Packet.createPacket(fb, Type.RoomInfo, ri);
            fb.finish(pk);
            byte[] bytes = fb.sizedByteArray();


            // 이제 이 정보는 호출한 클라이언트와 내가 참여하기로한 방에 있는 유저들에게 정보를 보낸다.
            for (int i = 0; i < roomList.get(current_room_number).RoomUser.size(); i++) {
                System.out.println("내가 들어간 방에 참여하고 있는 유저에게 정보 보냄(Roominfo) : " + roomList.get(current_room_number).RoomUser.get(i).Userid);
                HT_ConnectedAllUserInfo.get(roomList.get(current_room_number).RoomUser.get(i).ServerLoginKey).socket.writeAndFlush(bytes);
            }

            byte[] RoomListData = MakeRoomListData();
            for (ChannelId key : HT_ConnectedAllUserInfo.keySet()) {
                if (HT_ConnectedAllUserInfo.get(key).GameState == 0) { // 로비에 있는 경우
                    System.out.println(getClass().toString() + "로비에 있는 사람들에게 방정보 업데이트 된것을 보냄(Roomlist) : " + HT_ConnectedAllUserInfo.get(key).UserID);
                    SendRoomData(key, RoomListData);
                }
            }
        }
    }

    public void UpdateRoomList() {
        byte[] RoomListData = MakeRoomListData();
        for (ChannelId key : HT_ConnectedAllUserInfo.keySet()) {
            if (HT_ConnectedAllUserInfo.get(key).GameState == 0) { // 로비에 있는 경우
                System.out.println(getClass().toString() + "로비에 있는 사람들에게 방정보 업데이트 된것을 보냄(Roomlist) : " + HT_ConnectedAllUserInfo.get(key).UserID);
                SendRoomData(key, RoomListData);
            }
        }
    }
}
