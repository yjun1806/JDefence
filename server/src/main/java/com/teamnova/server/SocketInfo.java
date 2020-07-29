package com.teamnova.server;

import io.netty.channel.Channel;

public class SocketInfo {
    public Channel socket;
    public String UserID;
    public int GameState;
    public int MaxPoint;
    public int IDCode;
    public boolean IsLogin;
    public int Potion100;
    public int Potion50;
    public int Gold;


    SocketInfo(Channel socket, String UserID, int GameState, int Point, int IDCode, int Potion100, int Potion50, int Gold){
        this.socket = socket;
        this.UserID = UserID;
        this.GameState = GameState;
        this.MaxPoint = Point;
        this.IDCode = IDCode;
        this.IsLogin = false;
        this.Potion100 = Potion100;
        this.Potion50 = Potion50;
        this.Gold = Gold;
    }
}
