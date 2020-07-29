// automatically generated by the FlatBuffers compiler, do not modify

package com.teamnova.server.object;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class UserInfo extends Table {
  public static UserInfo getRootAsUserInfo(ByteBuffer _bb) { return getRootAsUserInfo(_bb, new UserInfo()); }
  public static UserInfo getRootAsUserInfo(ByteBuffer _bb, UserInfo obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public void __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; }
  public UserInfo __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public String ID() { int o = __offset(4); return o != 0 ? __string(o + bb_pos) : null; }
  public ByteBuffer IDAsByteBuffer() { return __vector_as_bytebuffer(4, 1); }
  public ByteBuffer IDInByteBuffer(ByteBuffer _bb) { return __vector_in_bytebuffer(_bb, 4, 1); }
  public int IDCode() { int o = __offset(6); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int code() { int o = __offset(8); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int MaxPoint() { int o = __offset(10); return o != 0 ? bb.getInt(o + bb_pos) : 0; }

  public static int createUserInfo(FlatBufferBuilder builder,
      int IDOffset,
      int IDCode,
      int code,
      int MaxPoint) {
    builder.startObject(4);
    UserInfo.addMaxPoint(builder, MaxPoint);
    UserInfo.addCode(builder, code);
    UserInfo.addIDCode(builder, IDCode);
    UserInfo.addID(builder, IDOffset);
    return UserInfo.endUserInfo(builder);
  }

  public static void startUserInfo(FlatBufferBuilder builder) { builder.startObject(4); }
  public static void addID(FlatBufferBuilder builder, int IDOffset) { builder.addOffset(0, IDOffset, 0); }
  public static void addIDCode(FlatBufferBuilder builder, int IDCode) { builder.addInt(1, IDCode, 0); }
  public static void addCode(FlatBufferBuilder builder, int code) { builder.addInt(2, code, 0); }
  public static void addMaxPoint(FlatBufferBuilder builder, int MaxPoint) { builder.addInt(3, MaxPoint, 0); }
  public static int endUserInfo(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
}

