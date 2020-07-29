package com.teamnova.server.RecvLogics;



import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.object.AllUser;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.Type;
import com.teamnova.server.object.UserInfo;


import java.util.Iterator;


import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelId;

import static com.teamnova.server.MainServer.HT_ConnectedAllUserInfo;

/*enum State {
    LOBBY, // 0
    ROOM, // 1
    GAMEPLAY, // 2
    LOGOUT // 3
}*/

public class SendUserInfo {


    public SendUserInfo(ChannelHandlerContext sock, ChannelId userKey) {
        Iterator<ChannelId> keys = HT_ConnectedAllUserInfo.keySet().iterator();


        // 이 클래스를 호출한 유저가 로그아웃 상태인경우, 호출한 유저를 제외한 모든 유저에게 메세지를 전달한다.
        if(HT_ConnectedAllUserInfo.get(userKey).GameState == 3){
            System.out.println("\n\n*----|");
            System.out.println("*----| Called User [ " + HT_ConnectedAllUserInfo.get(userKey).UserID+" ] Logout");
            System.out.println("*----|");

            FlatBufferBuilder flatBufferBuilder = new FlatBufferBuilder(8192);
            // 통째로 보내는 코드
            int ui[] = new int[HT_ConnectedAllUserInfo.size()];
            int i=0;
            while (keys.hasNext()){
                ChannelId key = keys.next();
                if(key != userKey){
                    ui[i] = UserInfo.createUserInfo(flatBufferBuilder,
                            flatBufferBuilder.createString(HT_ConnectedAllUserInfo.get(key).UserID),
                            HT_ConnectedAllUserInfo.get(key).IDCode,
                            HT_ConnectedAllUserInfo.get(key).GameState,
                            HT_ConnectedAllUserInfo.get(key).MaxPoint);
                }
                i++;
            }
            int au = AllUser.createAllListVector(flatBufferBuilder, ui);
            AllUser.startAllUser(flatBufferBuilder);
            AllUser.addAllList(flatBufferBuilder, au);
            int al = AllUser.endAllUser(flatBufferBuilder);
            int pk = Packet.createPacket(flatBufferBuilder, Type.AllUser, al);
            flatBufferBuilder.finish(pk);
            byte[] sendbyte = flatBufferBuilder.sizedByteArray();

            while (keys.hasNext()){
                ChannelId key = keys.next();
                if(key != userKey) {
                    HT_ConnectedAllUserInfo.get(key).socket.writeAndFlush(sendbyte);
                }
            }


        }

        // 게임 상태가 로비인 경우
        // 1. 최초로 로그인한경우
        // 2. 게임플레이를 끝내고 로비로 돌아오는 경우
        // 3. 게임방에서 나와 로비로 돌아온 경우
        // 유저 정보는 자신만 받는다.
        if(HT_ConnectedAllUserInfo.get(userKey).GameState == 0){
            System.out.println("\n\n*----|");
            System.out.println("*----| [ " + HT_ConnectedAllUserInfo.get(userKey).UserID+" ] Status : Lobby");
            System.out.println("*----|");

            FlatBufferBuilder flatBufferBuilder = new FlatBufferBuilder(8192);
            // 통째로 보내는 코드
            int ui[] = new int[HT_ConnectedAllUserInfo.size()]; // 유저 리스트를 담을 오프셋 배열
            int i=0;
            while (keys.hasNext()){
                ChannelId key = keys.next();
                // 배열에 유저리스트를 생성해 담아준다.
                ui[i] = UserInfo.createUserInfo(flatBufferBuilder,
                        flatBufferBuilder.createString(HT_ConnectedAllUserInfo.get(key).UserID),
                        HT_ConnectedAllUserInfo.get(key).IDCode,
                        HT_ConnectedAllUserInfo.get(key).GameState,
                        HT_ConnectedAllUserInfo.get(key).MaxPoint);
                i++;
            }
            int au = AllUser.createAllListVector(flatBufferBuilder, ui); // 유저리스트를 벡터화한다.
            AllUser.startAllUser(flatBufferBuilder); // 올유저에 담는다.
            AllUser.addAllList(flatBufferBuilder, au);
            int al = AllUser.endAllUser(flatBufferBuilder);
            int pk = Packet.createPacket(flatBufferBuilder, Type.AllUser, al);
            flatBufferBuilder.finish(pk);
            byte[] sendbyte = flatBufferBuilder.sizedByteArray();


            // 호출한 클라이언트에게만 데이터를 보낸다.
            // 즉, 이 경우는 다른상태에 있다가 로비로 돌아와서, 로비의 접속자리스트를 업데이트 하기위해 전체 유저데이터를 받는 경우이다.

            sock.writeAndFlush(sendbyte);
        }

    }
}
