// automatically generated by the FlatBuffers compiler, do not modify

package com.teamnova.server.object;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class GameReadyInfo extends Table {
  public static GameReadyInfo getRootAsGameReadyInfo(ByteBuffer _bb) { return getRootAsGameReadyInfo(_bb, new GameReadyInfo()); }
  public static GameReadyInfo getRootAsGameReadyInfo(ByteBuffer _bb, GameReadyInfo obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public void __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; }
  public GameReadyInfo __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int ReadyUserId() { int o = __offset(4); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public boolean ready() { int o = __offset(6); return o != 0 ? 0!=bb.get(o + bb_pos) : false; }
  public int ReadyRoomnumber() { int o = __offset(8); return o != 0 ? bb.getInt(o + bb_pos) : 0; }

  public static int createGameReadyInfo(FlatBufferBuilder builder,
      int ReadyUserId,
      boolean ready,
      int ReadyRoomnumber) {
    builder.startObject(3);
    GameReadyInfo.addReadyRoomnumber(builder, ReadyRoomnumber);
    GameReadyInfo.addReadyUserId(builder, ReadyUserId);
    GameReadyInfo.addReady(builder, ready);
    return GameReadyInfo.endGameReadyInfo(builder);
  }

  public static void startGameReadyInfo(FlatBufferBuilder builder) { builder.startObject(3); }
  public static void addReadyUserId(FlatBufferBuilder builder, int ReadyUserId) { builder.addInt(0, ReadyUserId, 0); }
  public static void addReady(FlatBufferBuilder builder, boolean ready) { builder.addBoolean(1, ready, false); }
  public static void addReadyRoomnumber(FlatBufferBuilder builder, int ReadyRoomnumber) { builder.addInt(2, ReadyRoomnumber, 0); }
  public static int endGameReadyInfo(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
}

