package com.teamnova.server.RecvLogics;


import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.object.MoveMonster;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.Type;


import io.netty.channel.ChannelId;

import static com.teamnova.server.MainServer.HT_ConnectedAllUserInfo;
import static com.teamnova.server.MainServer.roomList;

public class SendMonsterInfo {

    public void Send(int roomnumber, int MonsterIndex){
        // 방에 접속한 유저들에게 몬스터 정보 전달
        // 단 방장 제외, 왜냐면 방장은 클라이언트에서 몬스터를 컨트롤하는 역할을 하므로
        // 방장도 데이터를 받도록 변경, 체력같은건 서버쪽에서 동기화처리 해야하므로
        for(int i=0; i<roomList.get(roomnumber).RoomUser.size(); i++){
            ChannelId targetuser = roomList.get(roomnumber).RoomUser.get(i).ServerLoginKey;

            byte[] bytes = roomList.get(roomnumber).MonsterList.get(MonsterIndex).MonsterInfo();
            HT_ConnectedAllUserInfo.get(targetuser).socket.writeAndFlush(bytes);

        }
    }

    public int SpawnMonster(int roomnumber){
        // 이 경우는 몬스터를 스폰하는 경우, 이때는 방장도 몬스터 정보를 받는다.
        // 즉 몬스터의 스폰은 서버에서 전적으로 결정된다. 서버에서 클라이언트에게 몬스터를 스폰시키면
        // 그 몬스터의 AI를 컨트롤하는 클라이언트는 방장클라이언트가 된다.


        // 몬스터 스폰 과정
        /*
        * 우선 서버에서 몬스터를 만들어, 해당 방의 몬스터 리스트에 저장한다.
        * 그리고 해당 방에 있는 유저정보를 탐색해 스폰받았는지에 대한 변수가 false인 경우 이 메소드를 실행해 해당 유저에게 스폰데이터를 보낸다.
        * 스폰데이터를 받은 유저가 SpawnOK라는 메세지를 보내면, 해당 유저의 스폰변수를 true로 바꿔준다.
        * 이 과정을 반복해 해당 방에 있는 모든 유저의 스폰변수가 true가 되면 방장 클라이언트한테 몬스터를 움직여라 라는 메세지를 보낸다.
        * 방장 클라가 이 메세지를 받고 이에 대한 응답으로 MoveOK라는 메세지를 보내면 해당 방에 있는 모든 유저의 스폰변수를 false로 바꿔준후 다음 스폰을 시작한다.
        *
        * */

        int num = 0;
        for(int i=0; i<roomList.get(roomnumber).RoomUser.size(); i++) {
            ChannelId targetuser = roomList.get(roomnumber).RoomUser.get(i).ServerLoginKey;
            if(!roomList.get(roomnumber).RoomUser.get(i).IsMonsterSpawn) { // 몬스터를 아직 스폰하지 않은 경우 데이터를 보낸다.
                byte[] bytes = roomList.get(roomnumber).MonsterList.get(roomList.get(roomnumber).MonsterList.size() -1).MonsterSpawnData(); // 항상 마지막 몬스터 값을 보낸다.
                HT_ConnectedAllUserInfo.get(targetuser).socket.writeAndFlush(bytes);
            }else {
                num++; // 스폰된 유저의 수
            }

        }

        return num;
    }

    public void MoveMonster(int roomnumber){
        for(int i=0; i<roomList.get(roomnumber).RoomUser.size(); i++) {
            if(roomList.get(roomnumber).RoomUser.get(i).UseridCode == roomList.get(roomnumber).RoomOwner){
                ChannelId targetuser = roomList.get(roomnumber).RoomUser.get(i).ServerLoginKey;
                int monsterid = roomList.get(roomnumber).MonsterList.get(roomList.get(roomnumber).MonsterList.size()-1).name;
                FlatBufferBuilder fb = new FlatBufferBuilder(1024);
                int mm = MoveMonster.createMoveMonster(fb, monsterid);
                int pk = Packet.createPacket(fb, Type.MoveMonster, mm);
                fb.finish(pk);
                HT_ConnectedAllUserInfo.get(targetuser).socket.writeAndFlush(fb.sizedByteArray());
            }
        }
    }

}
