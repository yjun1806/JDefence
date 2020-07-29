// automatically generated by the FlatBuffers compiler, do not modify

package com.teamnova.server.object;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class RoomUserInfo extends Table {
  public static RoomUserInfo getRootAsRoomUserInfo(ByteBuffer _bb) { return getRootAsRoomUserInfo(_bb, new RoomUserInfo()); }
  public static RoomUserInfo getRootAsRoomUserInfo(ByteBuffer _bb, RoomUserInfo obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public void __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; }
  public RoomUserInfo __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public String userid() { int o = __offset(4); return o != 0 ? __string(o + bb_pos) : null; }
  public ByteBuffer useridAsByteBuffer() { return __vector_as_bytebuffer(4, 1); }
  public ByteBuffer useridInByteBuffer(ByteBuffer _bb) { return __vector_in_bytebuffer(_bb, 4, 1); }
  public int useridCode() { int o = __offset(6); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public boolean isready() { int o = __offset(8); return o != 0 ? 0!=bb.get(o + bb_pos) : false; }
  public int MaxPoint() { int o = __offset(10); return o != 0 ? bb.getInt(o + bb_pos) : 0; }

  public static int createRoomUserInfo(FlatBufferBuilder builder,
      int useridOffset,
      int useridCode,
      boolean isready,
      int MaxPoint) {
    builder.startObject(4);
    RoomUserInfo.addMaxPoint(builder, MaxPoint);
    RoomUserInfo.addUseridCode(builder, useridCode);
    RoomUserInfo.addUserid(builder, useridOffset);
    RoomUserInfo.addIsready(builder, isready);
    return RoomUserInfo.endRoomUserInfo(builder);
  }

  public static void startRoomUserInfo(FlatBufferBuilder builder) { builder.startObject(4); }
  public static void addUserid(FlatBufferBuilder builder, int useridOffset) { builder.addOffset(0, useridOffset, 0); }
  public static void addUseridCode(FlatBufferBuilder builder, int useridCode) { builder.addInt(1, useridCode, 0); }
  public static void addIsready(FlatBufferBuilder builder, boolean isready) { builder.addBoolean(2, isready, false); }
  public static void addMaxPoint(FlatBufferBuilder builder, int MaxPoint) { builder.addInt(3, MaxPoint, 0); }
  public static int endRoomUserInfo(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
}

