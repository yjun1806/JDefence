package com.teamnova.server.RecvLogics;


import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.object.GameState;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.Type;

import java.util.Iterator;

import io.netty.channel.ChannelId;

import static com.teamnova.server.MainServer.HT_ConnectedAllUserInfo;
import static com.teamnova.server.MainServer.roomList;


public class SendGameState {
    public SendGameState(){

    }

    public SendGameState(ChannelId userKey, String UserID) {
        // 이 메세지를 보낸다는 것은 내가 로그인을 했거나, 로그아웃을 했거나, 방에 참여했다는 것을
        // 로비에 있는 유저들에게 알리고 싶기 떄문!!
        // 즉, 로비에 있는 유저들만 이 정보를 받으면 된다.

        // 아니다. 모든 상태에 있는 유저들이 이 정보를 받아야 한다.
        // 왜냐하면 게임플레이중에 나가는 플레이어의 캐릭터를 지워주기 위해선
        // 상태의 변화를 알아야하기 때문

        Iterator<ChannelId> keys = HT_ConnectedAllUserInfo.keySet().iterator();
        FlatBufferBuilder fb = new FlatBufferBuilder(8192);
        while (keys.hasNext()){
            ChannelId key = keys.next();
            if(key == userKey) {
                // 내가보낸 정보코드를 내가 받을 필요 없으니 탐색에서 제외한다.
                continue;
            }
           /* if(HT_ConnectedAllUserInfo.get(key).GameState != 0){
                // 로비에 있지 않은 유저들도 받을 필요 없으므로, 전송 제외
                continue;
            }*/

            int id = fb.createString(UserID);
            GameState.startGameState(fb);
            GameState.addUserID(fb,id);
            GameState.addUserIDCode(fb, HT_ConnectedAllUserInfo.get(userKey).IDCode);
            GameState.addCode(fb, HT_ConnectedAllUserInfo.get(userKey).GameState);
            GameState.addMaxPoint(fb, HT_ConnectedAllUserInfo.get(userKey).MaxPoint);
            int gs = GameState.endGameState(fb);
            int pk = Packet.createPacket(fb, Type.GameState, gs);
            fb.finish(pk);

            byte[] bytes = fb.sizedByteArray();
            HT_ConnectedAllUserInfo.get(key).socket.writeAndFlush(bytes);

        }
    }

    // 서버에서 알리는 메세지를 띄우기 위한 코드를 보내는 부분이다.
    // 코드번호 10000이상으로 보낸다.
    public void SendMessageCode(int code, int roomnumber){

        for(int i=0; i<roomList.get(roomnumber).RoomUser.size(); i++){

            FlatBufferBuilder fb = new FlatBufferBuilder(8192);
            GameState.startGameState(fb);
            GameState.addCode(fb, code);
            int gs = GameState.endGameState(fb);
            int pk = Packet.createPacket(fb, Type.GameState, gs);
            fb.finish(pk);

            byte[] bytes = fb.sizedByteArray();
            HT_ConnectedAllUserInfo.get(roomList.get(roomnumber).RoomUser.get(i).ServerLoginKey).socket.writeAndFlush(bytes);

        }
    }
}
