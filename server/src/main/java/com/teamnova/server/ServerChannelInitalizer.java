package com.teamnova.server;


import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelPipeline;
import io.netty.channel.FixedRecvByteBufAllocator;
import io.netty.channel.socket.SocketChannel;
import io.netty.handler.codec.DelimiterBasedFrameDecoder;
import io.netty.handler.codec.Delimiters;
import io.netty.handler.codec.bytes.ByteArrayEncoder;

public class ServerChannelInitalizer extends ChannelInitializer<SocketChannel> {
    protected void initChannel(SocketChannel ch) throws Exception {
        ChannelPipeline cp = ch.pipeline();
        // 디코더 protobuf
        /* The connection is closed when there is no inbound traffic for N seconds */
        // 30초동안 아무런 입력이 없으면 연결을 닫는 핸들러
        //cp.addLast(new IdleStateHandler(30, 0, 0));
        //cp.addLast(new DelimiterBasedFrameDecoder(8192, Delimiters.lineDelimiter()));

//        cp.addLast(new StringEncoder());
//        cp.addLast(new StringDecoder());
        //cp.addLast(new ObjectDecoder( 1024 * 1024, ClassResolvers.weakCachingConcurrentResolver(this.getClass().getClassLoader())),new ObjectEncoder());
        //cp.addLast("frameDecoder", new DelimiterBasedFrameDecoder(80960, Delimiters.lineDelimiter()));
        ch.config().setRecvByteBufAllocator(new FixedRecvByteBufAllocator(1024*16)); // 입력받는 bytebuf의 사이즈를 늘려준다. 기본값은  1024바이트
        cp.addLast("bytesEncoder", new ByteArrayEncoder()); // 바이트배열을 보내기 위해 넣어주는 인코더
        cp.addLast(new FlatDecoder());
        cp.addLast(new RecvHandler());
    }
}
