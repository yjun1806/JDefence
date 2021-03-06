// automatically generated by the FlatBuffers compiler, do not modify

package com.teamnova.server.object;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class Location extends Table {
  public static Location getRootAsLocation(ByteBuffer _bb) { return getRootAsLocation(_bb, new Location()); }
  public static Location getRootAsLocation(ByteBuffer _bb, Location obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public void __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; }
  public Location __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public float X() { int o = __offset(4); return o != 0 ? bb.getFloat(o + bb_pos) : 0.0f; }
  public float Y() { int o = __offset(6); return o != 0 ? bb.getFloat(o + bb_pos) : 0.0f; }
  public float Z() { int o = __offset(8); return o != 0 ? bb.getFloat(o + bb_pos) : 0.0f; }

  public static int createLocation(FlatBufferBuilder builder,
      float X,
      float Y,
      float Z) {
    builder.startObject(3);
    Location.addZ(builder, Z);
    Location.addY(builder, Y);
    Location.addX(builder, X);
    return Location.endLocation(builder);
  }

  public static void startLocation(FlatBufferBuilder builder) { builder.startObject(3); }
  public static void addX(FlatBufferBuilder builder, float X) { builder.addFloat(0, X, 0.0f); }
  public static void addY(FlatBufferBuilder builder, float Y) { builder.addFloat(1, Y, 0.0f); }
  public static void addZ(FlatBufferBuilder builder, float Z) { builder.addFloat(2, Z, 0.0f); }
  public static int endLocation(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
}

