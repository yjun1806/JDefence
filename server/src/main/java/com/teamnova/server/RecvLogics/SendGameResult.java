package com.teamnova.server.RecvLogics;



import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.RoomUserInfoObj;
import com.teamnova.server.object.GameResult;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.Type;

import java.util.ArrayList;
import java.util.Collections;

import io.netty.channel.ChannelId;

import static com.teamnova.server.MainServer.HT_ConnectedAllUserInfo;
import static com.teamnova.server.MainServer.roomList;


public class SendGameResult {
    ArrayList<GameRanking> gameRankings;

    public SendGameResult(int Roomnumber){
        // 게임랭킹이라는 클래스 배열에 Roomnumber 방의 정보를 저장한다.
        gameRankings = new ArrayList<GameRanking>();

        //방의 인원수에 상관없이 무조건 4명의 데이터를 저장한다.
        for(int i =0; i< 4; i++){
            if(i >=roomList.get(Roomnumber).RoomUser.size()){ // 예를들어 인원이 2명인경우엔, 나머지 3,4번칸에 대한 값은 0으로 채운다.
                gameRankings.add(new GameRanking(null, 0));
            }else {
                gameRankings.add(new GameRanking(roomList.get(Roomnumber).RoomUser.get(i), CalPoint(Roomnumber, i))); // 1,2번 칸은 정상적으로 데이터를 계산해서 넣어준다.
            }
        }


        for(int i=0; i< roomList.get(Roomnumber).RoomUser.size(); i++){ // 방에 참여하고 있는 모든 유저에게 데이터를 전달한다.
            ChannelId targetID = roomList.get(Roomnumber).RoomUser.get(i).ServerLoginKey;
            byte[] bytes = RankSwap(gameRankings, i, Roomnumber).sizedByteArray(); // 랭킹을 정렬해서 바이트배열로 바꿔준다.
            HT_ConnectedAllUserInfo.get(targetID).socket.writeAndFlush(bytes);
        }

    }

    public int CalPoint(int Roomnumber, int index){
        // 전투에 따른 총 포인트를 계산하는 부분이다.

        int CurrentPoint = roomList.get(Roomnumber).RoomUser.get(index).CalPoint();
        int DefensePoint = roomList.get(Roomnumber).DuringDefenseTime * 10; // 디펜스시간 에따른 점수
        int LeftTowerHP = roomList.get(Roomnumber).LeftTowerHP; // 남은 타워체력에 따른 점수

        int result = DefensePoint + LeftTowerHP + CurrentPoint;
        roomList.get(Roomnumber).RoomUser.get(index).MaxPoint = result;
        return result;
    }

    public FlatBufferBuilder RankSwap(ArrayList<GameRanking> gr, int index, int Roomnumber){
        // 점수가 가장 높은순으로 배열을 정렬해준다.
        for(int i=0; i<gr.size()-1; i++){
            for(int j=i+1; j<gr.size(); j++){
                if(gr.get(i).ResultPoint < gr.get(j).ResultPoint){
                    Collections.swap(gr, i, j);
                }
            }
        }

        // 정렬된 데이터를 플랫버퍼에 담는다.
        FlatBufferBuilder fb = new FlatBufferBuilder();
        int rank1 = fb.createString(gameRankings.get(0).UserID);
        int rank2 = fb.createString(gameRankings.get(1).UserID);
        int rank3 = fb.createString(gameRankings.get(2).UserID);
        int rank4 = fb.createString(gameRankings.get(3).UserID);
        GameResult.startGameResult(fb);
        GameResult.addRank1ID(fb, rank1);
        GameResult.addRank2ID(fb, rank2);
        GameResult.addRank3ID(fb, rank3);
        GameResult.addRank4ID(fb, rank4);
        GameResult.addRank1Point(fb, gameRankings.get(0).ResultPoint);
        GameResult.addRank2Point(fb, gameRankings.get(1).ResultPoint);
        GameResult.addRank3Point(fb, gameRankings.get(2).ResultPoint);
        GameResult.addRank4Point(fb, gameRankings.get(3).ResultPoint);
        GameResult.addDuringDefenseTime(fb, roomList.get(Roomnumber).DuringDefenseTime);
        GameResult.addLeftTowerHP(fb, roomList.get(Roomnumber).LeftTowerHP);
        GameResult.addKilledMonster(fb, roomList.get(Roomnumber).RoomUser.get(index).KilledMonsterValue);
        GameResult.addTotalReceivedDamage(fb, roomList.get(Roomnumber).RoomUser.get(index).TotalReceivedDamage);
        GameResult.addTotalDied(fb, roomList.get(Roomnumber).RoomUser.get(index).TotalDiedValue);
        GameResult.addTotalDamageToMonster(fb, roomList.get(Roomnumber).RoomUser.get(index).TotalDamageToMonster);
        GameResult.addKilledMonsterRare(fb, roomList.get(Roomnumber).RoomUser.get(index).KilledMonsterValue_Rare);
        GameResult.addKilledMonsterBoss(fb, roomList.get(Roomnumber).RoomUser.get(index).KilledMonsterValue_Boss);
        int grt = GameResult.endGameResult(fb);
        int pk= Packet.createPacket(fb, Type.GameResult, grt);
        fb.finish(pk);


       /* System.out.println("정렬된 랭크값");
        System.out.println("1 : " + gr.get(0).UserID + " / " + gr.get(0).ResultPoint);
        System.out.println("2 : " + gr.get(1).UserID + " / " + gr.get(1).ResultPoint);
        System.out.println("3 : " + gr.get(2).UserID + " / " + gr.get(2).ResultPoint);
        System.out.println("4 : " + gr.get(3).UserID + " / " + gr.get(3).ResultPoint);*/

        return fb;
    }
}

class GameRanking{
    String UserID;
    int ResultPoint;

    public GameRanking(RoomUserInfoObj data, int resultPoint){
        if(data == null){
            this.UserID = "";
        }else {
            this.UserID = data.Userid;
        }
        this.ResultPoint = resultPoint;
    }
}