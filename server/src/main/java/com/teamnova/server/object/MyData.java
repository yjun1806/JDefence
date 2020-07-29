// automatically generated by the FlatBuffers compiler, do not modify

package com.teamnova.server.object;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class MyData extends Table {
  public static MyData getRootAsMyData(ByteBuffer _bb) { return getRootAsMyData(_bb, new MyData()); }
  public static MyData getRootAsMyData(ByteBuffer _bb, MyData obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public void __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; }
  public MyData __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int HP() { int o = __offset(4); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int Point() { int o = __offset(6); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int Potion100() { int o = __offset(8); return o != 0 ? bb.getInt(o + bb_pos) : 0; }
  public int Potion50() { int o = __offset(10); return o != 0 ? bb.getInt(o + bb_pos) : 0; }

  public static int createMyData(FlatBufferBuilder builder,
      int HP,
      int Point,
      int Potion100,
      int Potion50) {
    builder.startObject(4);
    MyData.addPotion50(builder, Potion50);
    MyData.addPotion100(builder, Potion100);
    MyData.addPoint(builder, Point);
    MyData.addHP(builder, HP);
    return MyData.endMyData(builder);
  }

  public static void startMyData(FlatBufferBuilder builder) { builder.startObject(4); }
  public static void addHP(FlatBufferBuilder builder, int HP) { builder.addInt(0, HP, 0); }
  public static void addPoint(FlatBufferBuilder builder, int Point) { builder.addInt(1, Point, 0); }
  public static void addPotion100(FlatBufferBuilder builder, int Potion100) { builder.addInt(2, Potion100, 0); }
  public static void addPotion50(FlatBufferBuilder builder, int Potion50) { builder.addInt(3, Potion50, 0); }
  public static int endMyData(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
}
