package com.teamnova.server.RecvLogics;



import com.google.flatbuffers.FlatBufferBuilder;

import java.io.IOException;
import java.net.SocketException;

import io.netty.channel.ChannelId;

import static com.teamnova.server.MainServer.HT_ConnectedAllUserInfo;
import static com.teamnova.server.MainServer.roomList;



public class SendCharacterInfo {
    public SendCharacterInfo() {
        //System.out.println("넘겨받은 데이터 캐릭터 정보 : " + playerdata.getPlayerName());
        //System.out.println("넘겨받은 데이터 캐릭터 정보\n " + playerdata);
    }



    public void SendPlayerData_In_Playing(int room_number, ChannelId Send_User, byte[] data) {
        for(int i=0; i< roomList.get(room_number).RoomUser.size(); i++){
            ChannelId targetID = roomList.get(room_number).RoomUser.get(i).ServerLoginKey;
            if(targetID != Send_User){ // 다른 클라에게 데이터를 보냄
                HT_ConnectedAllUserInfo.get(targetID).socket.writeAndFlush(data);
            }else { // 나 자신의 데이터를 받는 경우
                SendHealth(room_number, Send_User, i);
            }
        }
    }

    private void SendHealth(int room_number, ChannelId TargetID, int index){
        byte[] bytes = roomList.get(room_number).RoomUser.get(index).MyData();
        HT_ConnectedAllUserInfo.get(TargetID).socket.writeAndFlush(bytes);

    }
}
