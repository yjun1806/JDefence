package com.teamnova.server;


import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.object.Location;
import com.teamnova.server.object.MyData;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.PlayerData;
import com.teamnova.server.object.Position;
import com.teamnova.server.object.Rotation;
import com.teamnova.server.object.Type;
import com.teamnova.server.object.Velocity;

import io.netty.channel.ChannelId;

import static com.teamnova.server.MainServer.roomList;

public class RoomUserInfoObj {
    public ChannelId ServerLoginKey;
    public String Userid;
    public int UseridCode;
    public boolean isready;
    public boolean isloadingcomplete;
    //public GameModels.PlayerData playerData;
    public int PlayerHealth;
    public boolean IsPlayerAlive;
    public boolean IsPlayerAttacking;
    public boolean IsSKillUsing;
    public int SkillNumber;
    float loc_x, loc_y, loc_z;
    float roc_pitch, roc_roll, roc_yaw;
    float voc_x, voc_y, voc_z;
    int AttackingNumber;

    public int KilledMonsterValue; // 죽인 몬스터수(개인별)
    public int KilledMonsterValue_Rare; // 죽인 몬스터수(개인별)
    public int KilledMonsterValue_Boss; // 죽인 몬스터수(개인별)
    public int TotalReceivedDamage; // 총 받은 대미지(개인별)
    public int TotalDiedValue; // 총 죽은 횟수(개인별)
    public int TotalDamageToMonster; // 몬스터에게 준 총대미지(개인별)
    public int MaxPoint;
    public int CurrentPoint;

    public boolean IsMonsterSpawn = false;

    public boolean GameEndOK;

    public int Potion100;
    public int Potion50;
    public int Gold;
    public int GetGold;

    public RoomUserInfoObj(String clientID, int code, boolean b, int Maxp, ChannelId channelId, int Potion100, int Potion50, int Gold) {
        this.ServerLoginKey = channelId;
        this.Userid = clientID;
        this.UseridCode = code;
        this.isready = b;
        this.KilledMonsterValue = 0;
        this.KilledMonsterValue_Rare = 0;
        this.KilledMonsterValue_Boss = 0;
        this.TotalDamageToMonster = 0;
        this.TotalDiedValue = 0;
        this.TotalReceivedDamage = 0;
        this.MaxPoint = Maxp;
        this.CurrentPoint = 0;
        this.PlayerHealth = 1000; // 캐릭터 초기 체력 1000
        this.GameEndOK = false;
        this.Potion100 = Potion100;
        this.Potion50 = Potion50;
        this.Gold = Gold;
        this.GetGold = 0;
    }

    synchronized public void UpdatePlayingUserData(PlayerData Data){
        //this.PlayerHealth = Data.health();
        this.IsPlayerAlive = Data.IsAlive();
        this.IsPlayerAttacking = Data.IsAttacking();
        this.AttackingNumber = Data.AttackCountNumber();
        this.IsSKillUsing = Data.IsSkillUsing();
        this.SkillNumber = Data.SkillNumber();

        this.loc_x = Data.playerPosition().loc().X();
        this.loc_y = Data.playerPosition().loc().Y();
        this.loc_z = Data.playerPosition().loc().Z();

        this.roc_yaw = Data.playerPosition().rot().Yaw();
        this.roc_roll = Data.playerPosition().rot().Roll();
        this.roc_pitch = Data.playerPosition().rot().Pitch();

        this.voc_x = Data.playerPosition().voc().VX();
        this.voc_y = Data.playerPosition().voc().VY();
        this.voc_z = Data.playerPosition().voc().VZ();
    }

    public byte[] PlayerData(){
        FlatBufferBuilder fb = new FlatBufferBuilder(8192); // 버퍼 크기가 충분하지 않으면 데이터가 제대로 도착못한다.
        int loc = Location.createLocation(fb, loc_x, loc_y, loc_z);
        int rot = Rotation.createRotation(fb, roc_pitch, roc_roll, roc_yaw);
        int voc = Velocity.createVelocity(fb, voc_x, voc_y, voc_z);
        int pos = Position.createPosition(fb, loc, rot, voc);

        int pd = PlayerData.createPlayerData(fb,
                fb.createString(this.Userid),
                this.UseridCode,
                this.PlayerHealth,
                pos,
                this.IsPlayerAlive,
                this.IsPlayerAttacking,
                this.AttackingNumber,
                this.IsSKillUsing,
                this.SkillNumber); // 디펜스 시간과 타워에 대한 점수는 최종포인트에서 합산된다.
        int pk = Packet.createPacket(fb, Type.PlayerData, pd);
        fb.finish(pk);

        return fb.sizedByteArray();
    }

    public byte[] MyData(){
        FlatBufferBuilder fb = new FlatBufferBuilder(8192); // 버퍼 크기가 충분하지 않으면 데이터가 제대로 도착못한다.

        int pd = MyData.createMyData(fb,
                this.PlayerHealth, CalPoint(),
                this.Potion100,
                this.Potion50); // 캐릭터 정보를 본인이 받을때는 체력과 현재포인트만 받으면 된다.
        int pk = Packet.createPacket(fb, Type.MyData, pd);
        fb.finish(pk);

        return fb.sizedByteArray();
    }

    public int CalPoint(){
        int KillPoint = this.KilledMonsterValue * 200; // 죽인몬스터당 점수 1000
        int KillPoint_Rare = this.KilledMonsterValue_Rare * 500; // 죽인몬스터당 점수 1000
        int KillPoint_Boss = this.KilledMonsterValue_Boss * 1000; // 죽인몬스터당 점수 1000
        int DamagePoint = this.TotalDamageToMonster * 10; // 대미지 준 포인트당 점수 10
        int RecvDmgPoint = this.TotalReceivedDamage * -1; // 대미지 받은 포인트당 점수 1
        int DiedPoint = this.TotalDiedValue * -1000; // 죽으면 점수 -1000
        return  KillPoint + DamagePoint + RecvDmgPoint + DiedPoint +  + KillPoint_Rare + KillPoint_Boss;
    }
}
