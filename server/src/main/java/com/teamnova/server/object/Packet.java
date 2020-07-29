// automatically generated by the FlatBuffers compiler, do not modify

package com.teamnova.server.object;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class Packet extends Table {
  public static Packet getRootAsPacket(ByteBuffer _bb) { return getRootAsPacket(_bb, new Packet()); }
  public static Packet getRootAsPacket(ByteBuffer _bb, Packet obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public void __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; }
  public Packet __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public byte packetTypeType() { int o = __offset(4); return o != 0 ? bb.get(o + bb_pos) : 0; }
  public Table packetType(Table obj) { int o = __offset(6); return o != 0 ? __union(obj, o) : null; }

  public static int createPacket(FlatBufferBuilder builder,
      byte packet_type_type,
      int packet_typeOffset) {
    builder.startObject(2);
    Packet.addPacketType(builder, packet_typeOffset);
    Packet.addPacketTypeType(builder, packet_type_type);
    return Packet.endPacket(builder);
  }

  public static void startPacket(FlatBufferBuilder builder) { builder.startObject(2); }
  public static void addPacketTypeType(FlatBufferBuilder builder, byte packetTypeType) { builder.addByte(0, packetTypeType, 0); }
  public static void addPacketType(FlatBufferBuilder builder, int packetTypeOffset) { builder.addOffset(1, packetTypeOffset, 0); }
  public static int endPacket(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
  public static void finishPacketBuffer(FlatBufferBuilder builder, int offset) { builder.finish(offset); }
  public static void finishSizePrefixedPacketBuffer(FlatBufferBuilder builder, int offset) { builder.finishSizePrefixed(offset); }
}
