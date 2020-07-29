package com.teamnova.server.RecvLogics;


import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.SystemMessage;
import com.teamnova.server.object.SystemTime;
import com.teamnova.server.object.Type;

import io.netty.channel.ChannelId;

import static com.teamnova.server.MainServer.HT_ConnectedAllUserInfo;
import static com.teamnova.server.MainServer.roomList;

public class SendSystemMessage {
    public SendSystemMessage(){

    }

    public void Send(String msg, int type, int roomnumber){
        FlatBufferBuilder fb = new FlatBufferBuilder(8192);
        int sm = SystemMessage.createSystemMessage(fb, fb.createString(msg), type);
        int pk = Packet.createPacket(fb, Type.SystemMessage, sm);
        fb.finish(pk);
        byte[] bytes = fb.sizedByteArray();

        for(int i=0; i< roomList.get(roomnumber).RoomUser.size(); i++ ){
            HT_ConnectedAllUserInfo.get(roomList.get(roomnumber).RoomUser.get(i).ServerLoginKey).socket.writeAndFlush(bytes);
        }
    }

    public void SendSpecific(String msg, int type, ChannelId id){
        FlatBufferBuilder fb = new FlatBufferBuilder(8192);
        int sm = SystemMessage.createSystemMessage(fb, fb.createString(msg), type);
        int pk = Packet.createPacket(fb, Type.SystemMessage, sm);
        fb.finish(pk);
        byte[] bytes = fb.sizedByteArray();
        HT_ConnectedAllUserInfo.get(id).socket.writeAndFlush(bytes);
    }

    public void SendTime(int time, int roomnumber){
        FlatBufferBuilder fb = new FlatBufferBuilder(8192);
        int sm = SystemTime.createSystemTime(fb, time);
        int pk = Packet.createPacket(fb, Type.SystemTime, sm);
        fb.finish(pk);
        byte[] bytes = fb.sizedByteArray();

        for(int i=0; i< roomList.get(roomnumber).RoomUser.size(); i++ ){
            HT_ConnectedAllUserInfo.get(roomList.get(roomnumber).RoomUser.get(i).ServerLoginKey).socket.writeAndFlush(bytes);
        }
    }
}
