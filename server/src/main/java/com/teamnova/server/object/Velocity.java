// automatically generated by the FlatBuffers compiler, do not modify

package com.teamnova.server.object;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class Velocity extends Table {
  public static Velocity getRootAsVelocity(ByteBuffer _bb) { return getRootAsVelocity(_bb, new Velocity()); }
  public static Velocity getRootAsVelocity(ByteBuffer _bb, Velocity obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public void __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; }
  public Velocity __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public float VX() { int o = __offset(4); return o != 0 ? bb.getFloat(o + bb_pos) : 0.0f; }
  public float VY() { int o = __offset(6); return o != 0 ? bb.getFloat(o + bb_pos) : 0.0f; }
  public float VZ() { int o = __offset(8); return o != 0 ? bb.getFloat(o + bb_pos) : 0.0f; }

  public static int createVelocity(FlatBufferBuilder builder,
      float VX,
      float VY,
      float VZ) {
    builder.startObject(3);
    Velocity.addVZ(builder, VZ);
    Velocity.addVY(builder, VY);
    Velocity.addVX(builder, VX);
    return Velocity.endVelocity(builder);
  }

  public static void startVelocity(FlatBufferBuilder builder) { builder.startObject(3); }
  public static void addVX(FlatBufferBuilder builder, float VX) { builder.addFloat(0, VX, 0.0f); }
  public static void addVY(FlatBufferBuilder builder, float VY) { builder.addFloat(1, VY, 0.0f); }
  public static void addVZ(FlatBufferBuilder builder, float VZ) { builder.addFloat(2, VZ, 0.0f); }
  public static int endVelocity(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
}

