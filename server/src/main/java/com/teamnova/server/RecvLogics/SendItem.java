package com.teamnova.server.RecvLogics;

import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.object.BuyRequest;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.RefreshItem;
import com.teamnova.server.object.Type;

import io.netty.channel.ChannelId;

import static com.teamnova.server.MainServer.HT_ConnectedAllUserInfo;

public class SendItem {

    public void SendInventory(ChannelId channelId){
        System.out.println("Inventory Send!! : " + channelId);
        FlatBufferBuilder fb = new FlatBufferBuilder(8192);
        int ri = RefreshItem.createRefreshItem(fb, HT_ConnectedAllUserInfo.get(channelId).Gold, HT_ConnectedAllUserInfo.get(channelId).Potion100, HT_ConnectedAllUserInfo.get(channelId).Potion50);
        int pk = Packet.createPacket(fb, Type.RefreshItem, ri);
        fb.finish(pk);
        HT_ConnectedAllUserInfo.get(channelId).socket.writeAndFlush(fb.sizedByteArray());
    }

    public void SendBuyRequest(ChannelId channelId, boolean request){
        FlatBufferBuilder fb = new FlatBufferBuilder(8192);
        int br = BuyRequest.createBuyRequest(fb, request);
        int pk = Packet.createPacket(fb, Type.BuyRequest, br);
        fb.finish(pk);
        HT_ConnectedAllUserInfo.get(channelId).socket.writeAndFlush(fb.sizedByteArray());
    }
}
