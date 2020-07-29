package com.teamnova.server;



import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.RecvLogics.SaveResultDB;
import com.teamnova.server.RecvLogics.SendGameResult;
import com.teamnova.server.RecvLogics.SendItem;
import com.teamnova.server.RecvLogics.SendMonsterInfo;
import com.teamnova.server.RecvLogics.SendSystemMessage;
import com.teamnova.server.object.GameEnd;
import com.teamnova.server.object.GameStartInfo;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.Type;

import java.util.ArrayList;
import java.util.Random;

import static com.teamnova.server.MainServer.HT_ConnectedAllUserInfo;
import static com.teamnova.server.MainServer.roomList;
import static java.lang.Thread.sleep;


class GamePlayThread extends Thread {
    private final SendSystemMessage sendSystemMessage;
    boolean threadflag = true;
    int room_number;
    boolean gamestart = false;
    boolean MonsterSpawn = false;
    boolean endgame = false;


    public GamePlayThread(int room_number){
        this.room_number = room_number;
        sendSystemMessage = new SendSystemMessage();
    }

    @Override
    public void run() {
        //roomList.get(room_number);

        System.out.println("\n*----| GamePlayThreadStart!! : " + roomList.get(room_number).RoomUser.size());
        Runnable count = new GameCountRunnable(room_number);
        Runnable spawn = new MonsterSpawnRunnable(room_number);
        Thread thread = new Thread(count);
        Thread spawnthread = new Thread(spawn);
        boolean IsGameWin = false;

        while (threadflag){

            // 첫번째 카운팅, 즉 방에서 방장이 게임시작 버튼을 누르고
            // 3, 2, 1초 카운팅 후에 오픈레벨이 진행되도록 하는 부분이다.
            if(roomList.get(room_number).First_Count_In_Room){
                System.out.println("First Count Start!");
                Counting();
                // 여기까지 진행하면 이제 이 부분은 더 이상 진행하지 않도록 꺼준다.
                roomList.get(room_number).First_Count_In_Room = false;
            }
            // 게임레벨로 이동

            // 다른플레이어의 맵로딩이 완료되면 진행됨
            if(roomList.get(room_number).Second_Count_In_Game){

                System.out.println("Second Count Start!");
                Counting();
                roomList.get(room_number).Second_Count_In_Game = false;
                threadflag = false;
                gamestart = true;
                MonsterSpawn = true;

                try {
                    System.out.println("Spawn Waiting!!");
                    sleep(5000);
                    System.out.println("GameStart Monster Spawn Start!!");

                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            if(gamestart) {

                //System.out.println("최대 몬스터 등장 수 : " + MaxMonsterNumber);

                // 여기 까지 진행된것은 방에서 게임 플레이 버튼을 누르고, 모든 플레이어들이 게임맵에 들어와서 게임시작을 할 수 있게 된 상황
                // 이제 서로의 캐릭터 동기화 데이터를 계속 전송할 차례
                try {
                    sleep(1000);
                    new SendSystemMessage().Send("타워를 지키며 3분간 버티면 게임에서 승리합니다.", 1, room_number);
                    sleep(1000);
                    new SendSystemMessage().Send("타워의 체력이 0이 되면 패배합니다.", 1, room_number);
                    sleep(1000);
                    new SendSystemMessage().Send("곧 몬스터가 스폰됩니다.", 4, room_number);
                    sleep(1000);
                    thread.start();
                    spawnthread.start();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            while (gamestart){
                try {
                    sleep(100);
                    // 타워의 체력이 0인 경우, 게임 오버

                    // 타워체력 경고메세지 보내는 부분
                    if(roomList.get(room_number).FountainHealth <= 1500 && !roomList.get(room_number).TowerHPAlert50){
                        roomList.get(room_number).TowerHPAlert50 = true;
                        sendSystemMessage.Send("타워의 체력이 50% 미만입니다", 2, room_number);
                    }else if(roomList.get(room_number).FountainHealth <= 900 && !roomList.get(room_number).TowerHPAlert30){
                        roomList.get(room_number).TowerHPAlert30 = true;
                        sendSystemMessage.Send("타워의 체력이 30% 미만입니다", 2, room_number);
                    }else if(roomList.get(room_number).FountainHealth <= 300 && !roomList.get(room_number).TowerHPAlert10){
                        roomList.get(room_number).TowerHPAlert10 = true;
                        sendSystemMessage.Send("타워의 체력이 10% 미만입니다", 2, room_number);
                    }else if(roomList.get(room_number).FountainHealth <= 0){
                        System.out.println("Tower Destroy!! Game Lose!!");
                        roomList.get(room_number).LeftTowerHP = 0;
                        roomList.get(room_number).DuringDefenseTime = 180 - roomList.get(room_number).GameTime;
                        thread.interrupt();
                        spawnthread.interrupt();
                        gamestart = false;
                        endgame =true;
                        IsGameWin = false;
                    }

                    // 남은시간이 0인 경우, 게임 승리 -> 어쩄든 게임 끝

                    if(roomList.get(room_number).GameTime < 0){
                        System.out.println("Game Win!!");
                        // 게임에서 이긴경우
                        roomList.get(room_number).DuringDefenseTime = 180;
                        roomList.get(room_number).LeftTowerHP = roomList.get(room_number).FountainHealth;
                        thread.interrupt();
                        spawnthread.interrupt();
                        gamestart = false;
                        endgame = true;
                        IsGameWin = true;

                    }
                    // 남은시간 안내메세지 보내는 부분
                    if(roomList.get(room_number).GameTime <= 120 && !roomList.get(room_number).LeftTime120){
                        roomList.get(room_number).LeftTime120 = true;
                        sendSystemMessage.Send("승리까지 120초 남았습니다.", 3, room_number);
                    }
                    if(roomList.get(room_number).GameTime <= 60 && !roomList.get(room_number).LeftTime60){
                        roomList.get(room_number).LeftTime60 = true;
                        sendSystemMessage.Send("승리까지 60초 남았습니다.", 3, room_number);
                    }
                    if(roomList.get(room_number).GameTime <= 30 && !roomList.get(room_number).LeftTime30){
                        roomList.get(room_number).LeftTime30 = true;
                        sendSystemMessage.Send("승리까지 30초 남았습니다.", 3, room_number);
                    }

                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                } catch (NullPointerException e){
                    this.interrupt();
                }
            }

            // 클라에서 게임종료 메세지를 보낼때까지 계속 끝났다고 보낸다.
            while (!roomList.get(room_number).GameEnd && endgame){
                try {
                    System.out.println("Game End!! : IsGameWin? = " + IsGameWin);
                    sleep(500);
                    boolean EndFlag = true;
                    while (EndFlag){
                        int EndPlayerNum = EndPlayer(IsGameWin);
                        System.out.println("End Player : " + EndPlayerNum);
                        if(EndPlayerNum == roomList.get(room_number).RoomUser.size()){
                            roomList.get(room_number).GameEnd = true;
                            EndFlag = false;
                            System.out.println("All Player End!! : " + roomList.get(room_number).RoomUser.size());
                        }
                        sleep(200);
                    }
                   /* for(int i=0; i<roomList.get(room_number).RoomUser.size(); i++){
                        if(!roomList.get(room_number).RoomUser.get(i).GameEndOK) {
                            FlatBufferBuilder fb = new FlatBufferBuilder(1024);
                            int ge = GameEnd.createGameEnd(fb, IsGameWin);
                            int pk = Packet.createPacket(fb, Type.GameEnd, ge);
                            fb.finish(pk);

                            HT_ConnectedAllUserInfo.get(roomList.get(room_number).RoomUser.get(i).ServerLoginKey).socket.writeAndFlush(fb.sizedByteArray());
                        }
                    }*/
                    sleep(500);
                    System.out.println("Send Game Result");
                    new SendGameResult(room_number); // 게임이 끝나고 결과를 보내준다.

                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

        }

        for(int i=0; i< roomList.get(room_number).RoomUser.size(); i++){
            new SaveResultDB(roomList.get(room_number).RoomUser.get(i).ServerLoginKey, roomList.get(room_number).RoomUser.get(i).MaxPoint);
            try {
                sleep(100);
                new SaveResultDB().SaveItemToDB(roomList.get(room_number).RoomUser.get(i).ServerLoginKey);
                sleep(100);
                new SendItem().SendInventory(roomList.get(room_number).RoomUser.get(i).ServerLoginKey); // 해당 아이디가 현재 가진, 돈, 포션 개수를 보내준다.
                sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

        }



        System.out.println("*----| GamePlayThread Stop");
    }

    public int EndPlayer(boolean IsGameWin){
        int result = 0;
        for(int i=0; i<roomList.get(room_number).RoomUser.size(); i++){
            if(!roomList.get(room_number).RoomUser.get(i).GameEndOK){
                System.out.println("Game End Packet!! Send : " + roomList.get(room_number).RoomUser.get(i).Userid);
                FlatBufferBuilder fb = new FlatBufferBuilder(8192);
                int ge = GameEnd.createGameEnd(fb, IsGameWin);
                int pk = Packet.createPacket(fb, Type.GameEnd, ge);
                fb.finish(pk);
                HT_ConnectedAllUserInfo.get(roomList.get(room_number).RoomUser.get(i).ServerLoginKey).socket.writeAndFlush(fb.sizedByteArray());

            }else {
                result++;
            }

        }

        return result;

    }

    public void Counting(){
        for(int i=39; i>=7; i--){
            try {

                for (int j = 0; j < roomList.get(room_number).RoomUser.size(); j++) {
                    int count = i / 10;
                    FlatBufferBuilder fb = new FlatBufferBuilder(8192);
                    GameStartInfo.startGameStartInfo(fb);
                    GameStartInfo.addCount(fb, count);
                    GameStartInfo.addIscounting(fb, true);
                    int gs = GameStartInfo.endGameStartInfo(fb);
                    int pk = Packet.createPacket(fb, Type.GameStartInfo, gs);
                    fb.finish(pk);
                    byte[] bytes = fb.sizedByteArray();
                    HT_ConnectedAllUserInfo.get(roomList.get(room_number).RoomUser.get(j).ServerLoginKey).socket.writeAndFlush(bytes);
                    //System.out.println("시간보냄 : " + roomList.get(room_number).RoomUser.get(j).Userid + " / " + count + "s");
                }
                sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }


}

class GameCountRunnable implements Runnable{
    private SendSystemMessage sendSystemMessage = new SendSystemMessage();
    private int room_number;

    public GameCountRunnable(int room_number)
    {
        this.room_number = room_number;
    }

    public void run() {
        System.out.println("Game Count Thread Start");
        while (!Thread.currentThread().isInterrupted()){
            try {
                sendSystemMessage.SendTime(--roomList.get(room_number).GameTime, room_number);
                if(roomList.get(room_number).GameTime < 0)
                {
                    break;
                }
                sleep(1000);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }

    }
}

class MonsterSpawnRunnable implements Runnable{
    private int room_number;
    private int AliveMonsterNumber;
    private int MaxMonsterNumber;
    private Random rand = new Random();
    private int MonsterIndex = 10000;
    private ArrayList<Point> RandomMonsterSpawnLocation;
    private SendMonsterInfo sendMonsterInfo;


    public MonsterSpawnRunnable(int roomnumber){
        this.room_number = roomnumber;
        this.AliveMonsterNumber = 0;
        MaxMonsterNumber = roomList.get(room_number).RoomUser.size() * 3 + 1;
        sendMonsterInfo = new SendMonsterInfo();
        RandomMonsterSpawnLocation = new ArrayList<Point>();
        RandomMonsterSpawnLocation.add(new Point(-170.0f, -4740.0f, 440.0f));
        RandomMonsterSpawnLocation.add(new Point(-5540.0f, 6750.0f, 300.0f));
        RandomMonsterSpawnLocation.add(new Point(-1480.0f, 7880.0f, 630.0f));
        RandomMonsterSpawnLocation.add(new Point(940.0f, 6070.0f, 550.0f));
        RandomMonsterSpawnLocation.add(new Point(3680.0f, 5270.0f, 740.0f));
        RandomMonsterSpawnLocation.add(new Point(5640.0f, 4340.0f, 1080.0f));
        RandomMonsterSpawnLocation.add(new Point(6050.0f, 620.0f, 300.0f));
        RandomMonsterSpawnLocation.add(new Point(4930.0f, -2480.0f, 420.0f));
        RandomMonsterSpawnLocation.add(new Point(-6780.0f, 1610.0f, 510.0f));
        RandomMonsterSpawnLocation.add(new Point(1980.0f, -3930.0f, 610.0f));
        RandomMonsterSpawnLocation.add(new Point(-1910.0f, -5930.0f, 460.0f));
        RandomMonsterSpawnLocation.add(new Point(-4750.0f, -3400.0f, 290.0f));
        RandomMonsterSpawnLocation.add(new Point(-7290.0f, -2590.0f, 540.0f));
        RandomMonsterSpawnLocation.add(new Point(-6230.0f, -1150.0f, 500.0f));
        RandomMonsterSpawnLocation.add(new Point(-7190.0f, 1810.0f, 610.0f));
        RandomMonsterSpawnLocation.add(new Point(-7030.0f, 5530.0f, 330.0f));
        RandomMonsterSpawnLocation.add(new Point(-4610.0f, 7130.0f, 250.0f));
        RandomMonsterSpawnLocation.add(new Point(-1270.0f, 8000.0f, 760.0f));
    }

    public void run() {
        while (!Thread.currentThread().isInterrupted()){
            try {
                //System.out.println("현재까지 등장한 몬스터 수 : " + roomList.get(room_number).MonsterList.size());
                for (int i = 0; i < roomList.get(room_number).MonsterList.size(); i++) {
                    if (roomList.get(room_number).MonsterList.get(i).isAlive) {
                        AliveMonsterNumber++;
                    }
                }
                //System.out.println("Game Monster Alive Data : " + AliveMonsterNumber + "(" + roomList.get(room_number).MonsterList.size() + ")");
                int range = rand.nextInt(100);
                if (AliveMonsterNumber < MaxMonsterNumber) {
                    if (range >= 0 && range < 60) { // 일반몬스터 등장 확률 60%
                        AddMonsterData(1);
                    } else if (range >= 60 && range < 90) { // 레어몬스터 등장 확률 30%
                        AddMonsterData(2);
                    } else if (range >= 90 && range < 100) { // 보스몬스터 등장 확률 10%
                        AddMonsterData(3);
                    }
                    sleep(50);
                    // 몬스터를 스폰되었는지 확인하는 부분
                    // 계속 탐색하여 모든 클라가 몬스터를 스폰한 경우에만 while문을 벗어난다.
                    boolean SpawnFlag = true;
                    while (SpawnFlag) {
                        System.out.println("Spawn Check!!");
                        int spawnusernum = sendMonsterInfo.SpawnMonster(room_number);
                        if(spawnusernum == roomList.get(room_number).RoomUser.size()){
                            System.out.println("Spawn End!!");
                            SpawnFlag = false;
                        }
                        sleep(200);
                        /*for (int i = 0; i < roomList.get(room_number).RoomUser.size(); ) {
                            //System.out.println("Spawn Info : [" + roomList.get(room_number).RoomUser.get(i).Userid +"] : " + roomList.get(room_number).RoomUser.get(i).IsMonsterSpawn);
                            if (roomList.get(room_number).RoomUser.get(i).IsMonsterSpawn) {
                                if (i == roomList.get(room_number).RoomUser.size() - 1) {
                                    System.out.println("Spawn End!!");
                                    SpawnFlag = false;
                                }
                                i++;
                            }
                            // sleep(10);
                        }*/
                    }
                    // 스폰은 다 했으므로 다시 스폰하기위해 false로 만들어준다.
                    for (int i = 0; i < roomList.get(room_number).RoomUser.size(); i++) {
                        roomList.get(room_number).RoomUser.get(i).IsMonsterSpawn = false;
                    }
                    roomList.get(room_number).MonsterSpawnOK = false;
                    // 방장 클라에게 몬스터를 움직이라고 데이터를 보내는 부분.
                    while (!roomList.get(room_number).MonsterSpawnOK) {
                        System.out.println("Move Monster!!!");
                        // System.out.println("Send Move Monster!! To RoomOwner");
                        sendMonsterInfo.MoveMonster(room_number);
                        sleep(200);
                    }

                    System.out.println("Move End");
                    // 몬스터가 움직였으니 다시 false로 만들어준다.
                    roomList.get(room_number).MonsterSpawnOK = false;
                    sleep(50);
                }
                AliveMonsterNumber = 0;


            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }

    private void AddMonsterData(int MonsterType){
        Random rand = new Random();

        Monster WaveMonster = null;

        switch (MonsterType){
            case 1: // 일반몬스터
                WaveMonster = new Monster(MonsterType, MonsterIndex++, RandomMonsterSpawnLocation.get(rand.nextInt(RandomMonsterSpawnLocation.size())));
                System.out.println("*----| Normal Monster Spawn\n");
                break;
            case 2: // 레어몬스터
                WaveMonster = new Monster(MonsterType, MonsterIndex++, RandomMonsterSpawnLocation.get(rand.nextInt(RandomMonsterSpawnLocation.size())));
                System.out.println("*----| Rare Monster Spawn\n");
                break;
            case 3: // 보스몬스터
                WaveMonster = new Monster(MonsterType, MonsterIndex++, RandomMonsterSpawnLocation.get(rand.nextInt(RandomMonsterSpawnLocation.size())));
                System.out.println("*----| Boss Monster Spawn\n");
                break;
        }
        roomList.get(room_number).MonsterList.add(WaveMonster); // 방정보에 몬스터 정보 추가
        //sendMonsterInfo.AddMonsterData(room_number, WaveMonster);
    }
}