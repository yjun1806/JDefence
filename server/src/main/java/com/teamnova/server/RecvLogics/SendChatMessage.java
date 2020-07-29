package com.teamnova.server.RecvLogics;


import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.object.ChatMessage;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.Type;

import io.netty.channel.ChannelId;

import static com.teamnova.server.MainServer.HT_ConnectedAllUserInfo;

public class SendChatMessage {
    public SendChatMessage() {
    }

    public void Send(ChatMessage chatMessage, ChannelId userid) {
        for (ChannelId key : HT_ConnectedAllUserInfo.keySet()) {
            if (key != userid // 메세지 보낸사람 제외
                    && HT_ConnectedAllUserInfo.get(key).GameState == HT_ConnectedAllUserInfo.get(userid).GameState) // 나랑 게임 상태가 다른 사람 제외
            {
                System.out.println("보냄 : " + key);
                FlatBufferBuilder fb = new FlatBufferBuilder(8192);
                int cm = ChatMessage.createChatMessage(fb,
                        fb.createString(chatMessage.SenderID()),
                        fb.createString(chatMessage.ChatMsg()),
                        chatMessage.GameState());
                int pk = Packet.createPacket(fb, Type.ChatMessage, cm);
                fb.finish(pk);
                byte[] bytes = fb.sizedByteArray();
                HT_ConnectedAllUserInfo.get(key).socket.writeAndFlush(bytes);
            }
        }
    }
}
