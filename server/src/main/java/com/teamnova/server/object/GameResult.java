// automatically generated by the FlatBuffers compiler, do not modify

package com.teamnova.server.object;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class GameResult extends Table {
  public static GameResult getRootAsGameResult(ByteBuffer _bb) { return getRootAsGameResult(_bb, new GameResult()); }
  public static GameResult getRootAsGameResult(ByteBuffer _bb, GameResult obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public void __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; }
  public GameResult __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public String Rank1ID() { int o = __offset(4); return o != 0 ? __string(o + bb_pos) : null; }
  public ByteBuffer Rank1IDAsByteBuffer() { return __vector_as_bytebuffer(4, 1); }
  public ByteBuffer Rank1IDInByteBuffer(ByteBuffer _bb) { return __vector_in_bytebuffer(_bb, 4, 1); }
  public int Rank1Point() { int o = __offset(6); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public String Rank2ID() { int o = __offset(8); return o != 0 ? __string(o + bb_pos) : null; }
  public ByteBuffer Rank2IDAsByteBuffer() { return __vector_as_bytebuffer(8, 1); }
  public ByteBuffer Rank2IDInByteBuffer(ByteBuffer _bb) { return __vector_in_bytebuffer(_bb, 8, 1); }
  public int Rank2Point() { int o = __offset(10); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public String Rank3ID() { int o = __offset(12); return o != 0 ? __string(o + bb_pos) : null; }
  public ByteBuffer Rank3IDAsByteBuffer() { return __vector_as_bytebuffer(12, 1); }
  public ByteBuffer Rank3IDInByteBuffer(ByteBuffer _bb) { return __vector_in_bytebuffer(_bb, 12, 1); }
  public int Rank3Point() { int o = __offset(14); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public String Rank4ID() { int o = __offset(16); return o != 0 ? __string(o + bb_pos) : null; }
  public ByteBuffer Rank4IDAsByteBuffer() { return __vector_as_bytebuffer(16, 1); }
  public ByteBuffer Rank4IDInByteBuffer(ByteBuffer _bb) { return __vector_in_bytebuffer(_bb, 16, 1); }
  public int Rank4Point() { int o = __offset(18); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int DuringDefenseTime() { int o = __offset(20); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int LeftTowerHP() { int o = __offset(22); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int KilledMonster() { int o = __offset(24); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int TotalReceivedDamage() { int o = __offset(26); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int TotalDied() { int o = __offset(28); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int TotalDamageToMonster() { int o = __offset(30); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int KilledMonsterRare() { int o = __offset(32); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int KilledMonsterBoss() { int o = __offset(34); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int GetGold() { int o = __offset(36); return o != 0 ? bb.getInt(o + bb_pos) : 0; }

  public static int createGameResult(FlatBufferBuilder builder,
      int Rank1_IDOffset,
      int Rank1_Point,
      int Rank2_IDOffset,
      int Rank2_Point,
      int Rank3_IDOffset,
      int Rank3_Point,
      int Rank4_IDOffset,
      int Rank4_Point,
      int DuringDefenseTime,
      int LeftTowerHP,
      int KilledMonster,
      int TotalReceivedDamage,
      int TotalDied,
      int TotalDamageToMonster,
      int KilledMonster_Rare,
      int KilledMonster_Boss,
      int GetGold) {
    builder.startObject(17);
    GameResult.addGetGold(builder, GetGold);
    GameResult.addKilledMonsterBoss(builder, KilledMonster_Boss);
    GameResult.addKilledMonsterRare(builder, KilledMonster_Rare);
    GameResult.addTotalDamageToMonster(builder, TotalDamageToMonster);
    GameResult.addTotalDied(builder, TotalDied);
    GameResult.addTotalReceivedDamage(builder, TotalReceivedDamage);
    GameResult.addKilledMonster(builder, KilledMonster);
    GameResult.addLeftTowerHP(builder, LeftTowerHP);
    GameResult.addDuringDefenseTime(builder, DuringDefenseTime);
    GameResult.addRank4Point(builder, Rank4_Point);
    GameResult.addRank4ID(builder, Rank4_IDOffset);
    GameResult.addRank3Point(builder, Rank3_Point);
    GameResult.addRank3ID(builder, Rank3_IDOffset);
    GameResult.addRank2Point(builder, Rank2_Point);
    GameResult.addRank2ID(builder, Rank2_IDOffset);
    GameResult.addRank1Point(builder, Rank1_Point);
    GameResult.addRank1ID(builder, Rank1_IDOffset);
    return GameResult.endGameResult(builder);
  }

  public static void startGameResult(FlatBufferBuilder builder) { builder.startObject(17); }
  public static void addRank1ID(FlatBufferBuilder builder, int Rank1IDOffset) { builder.addOffset(0, Rank1IDOffset, 0); }
  public static void addRank1Point(FlatBufferBuilder builder, int Rank1Point) { builder.addInt(1, Rank1Point, 0); }
  public static void addRank2ID(FlatBufferBuilder builder, int Rank2IDOffset) { builder.addOffset(2, Rank2IDOffset, 0); }
  public static void addRank2Point(FlatBufferBuilder builder, int Rank2Point) { builder.addInt(3, Rank2Point, 0); }
  public static void addRank3ID(FlatBufferBuilder builder, int Rank3IDOffset) { builder.addOffset(4, Rank3IDOffset, 0); }
  public static void addRank3Point(FlatBufferBuilder builder, int Rank3Point) { builder.addInt(5, Rank3Point, 0); }
  public static void addRank4ID(FlatBufferBuilder builder, int Rank4IDOffset) { builder.addOffset(6, Rank4IDOffset, 0); }
  public static void addRank4Point(FlatBufferBuilder builder, int Rank4Point) { builder.addInt(7, Rank4Point, 0); }
  public static void addDuringDefenseTime(FlatBufferBuilder builder, int DuringDefenseTime) { builder.addInt(8, DuringDefenseTime, 0); }
  public static void addLeftTowerHP(FlatBufferBuilder builder, int LeftTowerHP) { builder.addInt(9, LeftTowerHP, 0); }
  public static void addKilledMonster(FlatBufferBuilder builder, int KilledMonster) { builder.addInt(10, KilledMonster, 0); }
  public static void addTotalReceivedDamage(FlatBufferBuilder builder, int TotalReceivedDamage) { builder.addInt(11, TotalReceivedDamage, 0); }
  public static void addTotalDied(FlatBufferBuilder builder, int TotalDied) { builder.addInt(12, TotalDied, 0); }
  public static void addTotalDamageToMonster(FlatBufferBuilder builder, int TotalDamageToMonster) { builder.addInt(13, TotalDamageToMonster, 0); }
  public static void addKilledMonsterRare(FlatBufferBuilder builder, int KilledMonsterRare) { builder.addInt(14, KilledMonsterRare, 0); }
  public static void addKilledMonsterBoss(FlatBufferBuilder builder, int KilledMonsterBoss) { builder.addInt(15, KilledMonsterBoss, 0); }
  public static void addGetGold(FlatBufferBuilder builder, int GetGold) { builder.addInt(16, GetGold, 0); }
  public static int endGameResult(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
}

