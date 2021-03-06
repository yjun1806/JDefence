// automatically generated by the FlatBuffers compiler, do not modify

package com.teamnova.server.object;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class RefreshItem extends Table {
  public static RefreshItem getRootAsRefreshItem(ByteBuffer _bb) { return getRootAsRefreshItem(_bb, new RefreshItem()); }
  public static RefreshItem getRootAsRefreshItem(ByteBuffer _bb, RefreshItem obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public void __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; }
  public RefreshItem __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int CurrentGold() { int o = __offset(4); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int CurrentPotion100() { int o = __offset(6); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int CurrentPotion50() { int o = __offset(8); return o != 0 ? bb.getInt(o + bb_pos) : 0; }

  public static int createRefreshItem(FlatBufferBuilder builder,
      int CurrentGold,
      int CurrentPotion100,
      int CurrentPotion50) {
    builder.startObject(3);
    RefreshItem.addCurrentPotion50(builder, CurrentPotion50);
    RefreshItem.addCurrentPotion100(builder, CurrentPotion100);
    RefreshItem.addCurrentGold(builder, CurrentGold);
    return RefreshItem.endRefreshItem(builder);
  }

  public static void startRefreshItem(FlatBufferBuilder builder) { builder.startObject(3); }
  public static void addCurrentGold(FlatBufferBuilder builder, int CurrentGold) { builder.addInt(0, CurrentGold, 0); }
  public static void addCurrentPotion100(FlatBufferBuilder builder, int CurrentPotion100) { builder.addInt(1, CurrentPotion100, 0); }
  public static void addCurrentPotion50(FlatBufferBuilder builder, int CurrentPotion50) { builder.addInt(2, CurrentPotion50, 0); }
  public static int endRefreshItem(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
}

