// automatically generated by the FlatBuffers compiler, do not modify

package com.teamnova.server.object;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class RoomInfo extends Table {
  public static RoomInfo getRootAsRoomInfo(ByteBuffer _bb) { return getRootAsRoomInfo(_bb, new RoomInfo()); }
  public static RoomInfo getRootAsRoomInfo(ByteBuffer _bb, RoomInfo obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public void __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; }
  public RoomInfo __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public String RoomName() { int o = __offset(4); return o != 0 ? __string(o + bb_pos) : null; }
  public ByteBuffer RoomNameAsByteBuffer() { return __vector_as_bytebuffer(4, 1); }
  public ByteBuffer RoomNameInByteBuffer(ByteBuffer _bb) { return __vector_in_bytebuffer(_bb, 4, 1); }
  public int RoomNumber() { int o = __offset(6); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int MaxUserNumber() { int o = __offset(8); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int CurrentUserNumber() { int o = __offset(10); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public String Password() { int o = __offset(12); return o != 0 ? __string(o + bb_pos) : null; }
  public ByteBuffer PasswordAsByteBuffer() { return __vector_as_bytebuffer(12, 1); }
  public ByteBuffer PasswordInByteBuffer(ByteBuffer _bb) { return __vector_in_bytebuffer(_bb, 12, 1); }
  public int RoomOwner() { int o = __offset(14); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public boolean IsThisRoomPlayingGame() { int o = __offset(16); return o != 0 ? 0!=bb.get(o + bb_pos) : false; }
  public RoomUserInfo userList(int j) { return userList(new RoomUserInfo(), j); }
  public RoomUserInfo userList(RoomUserInfo obj, int j) { int o = __offset(18); return o != 0 ? obj.__assign(__indirect(__vector(o) + j * 4), bb) : null; }
  public int userListLength() { int o = __offset(18); return o != 0 ? __vector_len(o) : 0; }
  public int FountainHealth() { int o = __offset(20); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public boolean IsThisRoomLock() { int o = __offset(22); return o != 0 ? 0!=bb.get(o + bb_pos) : false; }

  public static int createRoomInfo(FlatBufferBuilder builder,
      int RoomNameOffset,
      int RoomNumber,
      int MaxUserNumber,
      int CurrentUserNumber,
      int PasswordOffset,
      int RoomOwner,
      boolean IsThisRoomPlayingGame,
      int user_listOffset,
      int FountainHealth,
      boolean IsThisRoomLock) {
    builder.startObject(10);
    RoomInfo.addFountainHealth(builder, FountainHealth);
    RoomInfo.addUserList(builder, user_listOffset);
    RoomInfo.addRoomOwner(builder, RoomOwner);
    RoomInfo.addPassword(builder, PasswordOffset);
    RoomInfo.addCurrentUserNumber(builder, CurrentUserNumber);
    RoomInfo.addMaxUserNumber(builder, MaxUserNumber);
    RoomInfo.addRoomNumber(builder, RoomNumber);
    RoomInfo.addRoomName(builder, RoomNameOffset);
    RoomInfo.addIsThisRoomLock(builder, IsThisRoomLock);
    RoomInfo.addIsThisRoomPlayingGame(builder, IsThisRoomPlayingGame);
    return RoomInfo.endRoomInfo(builder);
  }

  public static void startRoomInfo(FlatBufferBuilder builder) { builder.startObject(10); }
  public static void addRoomName(FlatBufferBuilder builder, int RoomNameOffset) { builder.addOffset(0, RoomNameOffset, 0); }
  public static void addRoomNumber(FlatBufferBuilder builder, int RoomNumber) { builder.addInt(1, RoomNumber, 0); }
  public static void addMaxUserNumber(FlatBufferBuilder builder, int MaxUserNumber) { builder.addInt(2, MaxUserNumber, 0); }
  public static void addCurrentUserNumber(FlatBufferBuilder builder, int CurrentUserNumber) { builder.addInt(3, CurrentUserNumber, 0); }
  public static void addPassword(FlatBufferBuilder builder, int PasswordOffset) { builder.addOffset(4, PasswordOffset, 0); }
  public static void addRoomOwner(FlatBufferBuilder builder, int RoomOwner) { builder.addInt(5, RoomOwner, 0); }
  public static void addIsThisRoomPlayingGame(FlatBufferBuilder builder, boolean IsThisRoomPlayingGame) { builder.addBoolean(6, IsThisRoomPlayingGame, false); }
  public static void addUserList(FlatBufferBuilder builder, int userListOffset) { builder.addOffset(7, userListOffset, 0); }
  public static int createUserListVector(FlatBufferBuilder builder, int[] data) { builder.startVector(4, data.length, 4); for (int i = data.length - 1; i >= 0; i--) builder.addOffset(data[i]); return builder.endVector(); }
  public static void startUserListVector(FlatBufferBuilder builder, int numElems) { builder.startVector(4, numElems, 4); }
  public static void addFountainHealth(FlatBufferBuilder builder, int FountainHealth) { builder.addInt(8, FountainHealth, 0); }
  public static void addIsThisRoomLock(FlatBufferBuilder builder, boolean IsThisRoomLock) { builder.addBoolean(9, IsThisRoomLock, false); }
  public static int endRoomInfo(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
}
