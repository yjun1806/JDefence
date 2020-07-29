package com.teamnova.server;


import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.object.Location;
import com.teamnova.server.object.MonsterData;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.SpawnMonsterData;
import com.teamnova.server.object.Type;

public class Monster {
    public float loc_x, loc_y, loc_z;
    public int health;
    public int MonsterType; // 1 : 노멀, 2 : 레어, 3: 유니크(보스)
    public boolean isAlive;
    public boolean isAttacking;
    public int AttackingNumber;
    public int Damage;
    public int name;

    public Monster(int MonsterType, int MonsterName, Point point){
        System.out.println("*----| MonsterInit - Name = [ " + MonsterName + " ], SpawnLocation = [ " + point.x +" / "+ point.y+ " / " + point.z + " ]");
        this.loc_x = point.x; // 초기 몬스터 등장 위치
        this.loc_y = point.y;
        this.loc_z = point.z;

        isAlive = true;
        isAttacking = false;
        AttackingNumber = 0;
        this.MonsterType = MonsterType;
        this.name = MonsterName;

        switch (MonsterType) // 몬스터 타입에 따라 능력치 차등
        {
            case 1: // 일반몬스터
                this.health = 100;
                this.Damage = 50;
                break;
            case 2: // 레어몬스터
                this.health = 200;
                this.Damage = 100;
                break;
            case 3: // 보스몬스터
                this.health = 400;
                this.Damage = 200;
                break;
        }
    }

    public byte[] MonsterInfo(){
        FlatBufferBuilder fb = new FlatBufferBuilder(1024*20);
        int loc = Location.createLocation(fb, loc_x, loc_y, loc_z);

        int md =  MonsterData.createMonsterData(fb,
                this.name,
                this.health,
                loc,
                this.isAlive,
                this.isAttacking,
                this.AttackingNumber,
                this.MonsterType);
        int pk = Packet.createPacket(fb, Type.MonsterData, md);
        fb.finish(pk);
        return fb.sizedByteArray();
    }

    public byte[] MonsterSpawnData(){
        FlatBufferBuilder fb = new FlatBufferBuilder(1024*20);
        int loc = Location.createLocation(fb, loc_x, loc_y, loc_z);

        int md =  SpawnMonsterData.createSpawnMonsterData(fb,
                this.name,
                this.health,
                loc,
                this.MonsterType,
                false);
        int pk = Packet.createPacket(fb, Type.SpawnMonsterData, md);
        fb.finish(pk);
        return fb.sizedByteArray();
    }

    public void UpdateMonsterData(MonsterData data){
        this.loc_x = data.monsterLocation().X();
        this.loc_y = data.monsterLocation().Y();
        this.loc_z = data.monsterLocation().Z();

        this.isAttacking = data.IsAttacking();
        this.AttackingNumber = data.AttackCountNumber();

    }

}

