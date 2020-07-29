package com.teamnova.server;

import java.util.ArrayList;

public class GameRoomInfo {
    public String RoomName;
    public int RoomNumber;
    public int MaxRoomUserNumber;
    public int CurrentUserNumber;
    public String Password;
    public int RoomOwner;
    public ArrayList<RoomUserInfoObj> RoomUser;
    public ArrayList<Monster> MonsterList;
    public boolean Is_This_Room_Game_Start;
    public boolean First_Count_In_Room;
    public boolean Second_Count_In_Game;
    public int FountainHealth;
    public int GameTime;
    public boolean TowerHPAlert50;
    public boolean TowerHPAlert30;
    public boolean TowerHPAlert10;
    public boolean LeftTime120;
    public boolean LeftTime60;
    public boolean LeftTime30;

    public int DuringDefenseTime; // 총 디펜스 진행시간
    public int LeftTowerHP; // 남은 타워 체력

    public boolean IsThisRoomLock;

    public boolean MonsterSpawnOK;

    public boolean GameEnd;
    public GameRoomInfo(){
        RoomUser = new ArrayList<RoomUserInfoObj>();
        MonsterList = new ArrayList<Monster>();
        this.Is_This_Room_Game_Start = false;
        this.First_Count_In_Room = false;
        this.Second_Count_In_Game = false;
        this.FountainHealth = 3000; // 타워체력
        this.GameTime = 120; // 게임진행시간
        this.TowerHPAlert10 = false;
        this.TowerHPAlert30 = false;
        this.TowerHPAlert50 = false;
        this.LeftTime30 = false;
        this.LeftTime60 = false;
        this.LeftTime120 = false;
        this.DuringDefenseTime = 0;
        this.LeftTowerHP = 3000;
        this.IsThisRoomLock = false;
        this.Password = null;
        this.MonsterSpawnOK = false;
        this.GameEnd = false;
    }
}

