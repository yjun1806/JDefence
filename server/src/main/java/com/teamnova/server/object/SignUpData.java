// automatically generated by the FlatBuffers compiler, do not modify

package com.teamnova.server.object;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class SignUpData extends Table {
  public static SignUpData getRootAsSignUpData(ByteBuffer _bb) { return getRootAsSignUpData(_bb, new SignUpData()); }
  public static SignUpData getRootAsSignUpData(ByteBuffer _bb, SignUpData obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public void __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; }
  public SignUpData __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public String ID() { int o = __offset(4); return o != 0 ? __string(o + bb_pos) : null; }
  public ByteBuffer IDAsByteBuffer() { return __vector_as_bytebuffer(4, 1); }
  public ByteBuffer IDInByteBuffer(ByteBuffer _bb) { return __vector_in_bytebuffer(_bb, 4, 1); }
  public String Password() { int o = __offset(6); return o != 0 ? __string(o + bb_pos) : null; }
  public ByteBuffer PasswordAsByteBuffer() { return __vector_as_bytebuffer(6, 1); }
  public ByteBuffer PasswordInByteBuffer(ByteBuffer _bb) { return __vector_in_bytebuffer(_bb, 6, 1); }
  public boolean IsSignUpPossible() { int o = __offset(8); return o != 0 ? 0!=bb.get(o + bb_pos) : false; }

  public static int createSignUpData(FlatBufferBuilder builder,
      int IDOffset,
      int PasswordOffset,
      boolean IsSignUpPossible) {
    builder.startObject(3);
    SignUpData.addPassword(builder, PasswordOffset);
    SignUpData.addID(builder, IDOffset);
    SignUpData.addIsSignUpPossible(builder, IsSignUpPossible);
    return SignUpData.endSignUpData(builder);
  }

  public static void startSignUpData(FlatBufferBuilder builder) { builder.startObject(3); }
  public static void addID(FlatBufferBuilder builder, int IDOffset) { builder.addOffset(0, IDOffset, 0); }
  public static void addPassword(FlatBufferBuilder builder, int PasswordOffset) { builder.addOffset(1, PasswordOffset, 0); }
  public static void addIsSignUpPossible(FlatBufferBuilder builder, boolean IsSignUpPossible) { builder.addBoolean(2, IsSignUpPossible, false); }
  public static int endSignUpData(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
}
