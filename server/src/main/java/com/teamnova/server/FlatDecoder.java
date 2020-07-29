package com.teamnova.server;

import com.teamnova.server.object.Packet;

import java.nio.ByteBuffer;
import java.util.List;

import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.ByteToMessageDecoder;

public class FlatDecoder extends ByteToMessageDecoder { // 입력받은 바이트를 메세지로 변환해주는 클래스, 서버에 들어오는 모든 데이터가 여기를 거쳐간다.
    protected void decode(ChannelHandlerContext ctx, ByteBuf byteBuf, List<Object> list) throws Exception {
        int length = byteBuf.readableBytes();
        byte[] bytes = new byte[length];
        byteBuf.readBytes(bytes);

        ByteBuffer byteBuffer = ByteBuffer.wrap(bytes);
        Packet packet = Packet.getRootAsPacket(byteBuffer); // 저장한 버퍼를 플랫버퍼의 패킷테이블로 변환

        list.add(packet); // 이 리스트는 입력받은 데이터에 대한 오브젝트 리스트이다. 데이터가 여기에 계속 쌓인다.
    }
}
