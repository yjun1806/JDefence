package com.teamnova.server;

import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;

public class NovaServerHanlder extends ChannelInboundHandlerAdapter {
    // 채널을 읽을 때 동작할 코드를 정의 합니다.



    //메세지가 들어올떄마다 호출된다
    public void channelRead(ChannelHandlerContext ctx, Object msg)
            throws Exception {

        //System.out.println("입력받은 데이터 \n" + msg.toString());
        //LoginDataProtos.LoginPacket readMessage = (LoginDataProtos.LoginPacket) msg;

        //3. 수신된 데이터를 가지고 있는 네티의 바이트 버퍼 객체로 부터 문자열 객체를 읽어온다.
        //System.out.println("수신한 문자열 ["+readMessage.getID() +"]" + readMessage.getPassword());

        /*switch (readMessage.get){
            // 전달받은 데이터 타입이 로그인인 경우
            case Login:



                break;
                // 전달받은 데이터 타입이 회원가입인 경우
            case SignUp:

                MemberDataProtos.MemberData.Type_SignUp Signupdata = readMessage.getTSignUp();
                Connection conn = null;
                Statement stmt = null; // 데이터를 전송하는 객체
                try{
                    Class.forName(JDBC_DRIVER);
                    conn = DriverManager.getConnection(DB_URL,USERNAME,PASSWORD); // 데이터 베이스 연결
                    System.out.println("\n- MySQL Connection");
                    stmt = conn.createStatement();

                    // member란 테이블에서 gamepw, gameid 컬럼을 찾는다.
                    String sql = "SELECT gamepw, gameid FROM member where gameid = ?"; // sql문을 만들어준다.
                    PreparedStatement pstmt = null;
                    ResultSet rs = null;
                    try {
                        pstmt = conn.prepareStatement(sql);
                        pstmt.setString(1, Signupdata.getSignUpID());
                        rs = pstmt.executeQuery();

                        rs.last();
                        System.out.println("크기 : " + rs.getRow());

                        if(rs.getRow() == 0){ // 아이디 검색결과 없다. 가입가능
                            System.out.println("\n\n*------| 아이디 검색 결과");
                            System.out.println("*------| "+Signupdata.getSignUpID() + "은 회원목록에 없는 아이디입니다.");
                            System.out.println("*------| 회원가입을 진행합니다.");
                            // 회원가입 처리부분
                            String insertsql = "insert into member (gamepw, gameid) value (?, ?)";
                            //PreparedStatement pstmt = null;
                            try{
                                pstmt = conn.prepareStatement(insertsql);
                                pstmt.setString(1, Signupdata.getPassword());
                                pstmt.setString(2, Signupdata.getSignUpID());
                                pstmt.executeUpdate();
                                System.out.println("*------| 회원가입 성공!");

                            }catch (SQLException e){
                                e.printStackTrace();
                            }finally {
                                try {
                                    if(pstmt != null && !pstmt.isClosed())
                                        pstmt.close();
                                }catch (SQLException e){
                                    e.printStackTrace();
                                }
                            }


                            MemberDataProtos.MemberData.Type_SignUp SendingData = MemberDataProtos.MemberData.newBuilder()
                                    .setType(MemberDataProtos.MemberData.Type.SignUp).getTSignUpBuilder().setHaveidInDB(false).build();

                            ChannelFuture cf = ctx.writeAndFlush(SendingData);
                            cf.addListener(new ChannelFutureListener() {
                                @Override
                                public void operationComplete(ChannelFuture future) throws Exception {
                                    if(future.isSuccess()) {
                                        System.out.println("*------| 서버->클라이언트 : 전송 성공");
                                    }
                                    else {
                                        System.out.println("*------| 서버->클라이언트 : 전송 실패");
                                    }
                                }
                            });


                        }else {
                            System.out.println("\n\n*------| 아이디 검색 결과");
                            System.out.println("*------| "+Signupdata.getSignUpID() + "은 회원목록에 있는 아이디입니다.");
                            System.out.println("*------| 회원가입을 할 수 없는 아이디 입니다.");
                            ////////////////////////
                            MemberDataProtos.MemberData.Type_SignUp SendingData = MemberDataProtos.MemberData.newBuilder()
                                    .setType(MemberDataProtos.MemberData.Type.SignUp).getTSignUpBuilder().setHaveidInDB(true).build();

                            ChannelFuture cf = ctx.writeAndFlush(SendingData);
                            cf.addListener(new ChannelFutureListener() {
                                @Override
                                public void operationComplete(ChannelFuture future) throws Exception {
                                    if(future.isSuccess()) {
                                        System.out.println("서버에서 전송 성공");
                                    }
                                    else {
                                        System.out.println("서버에서 전송  실패");
                                    }
                                }
                            });
                        }
                    } catch (SQLException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    } finally {
                        try {
                            if (pstmt != null && !pstmt.isClosed())
                                pstmt.close();
                        } catch (SQLException e) {
                            // TODO Auto-generated catch block
                            e.printStackTrace();
                        }
                    }

                    rs.close();
                    stmt.close();
                    conn.close();
                }catch(SQLException se1){
                    se1.printStackTrace();
                }catch(Exception ex){
                    ex.printStackTrace();
                }finally{
                    try{
                        if(stmt!=null)
                            stmt.close();
                    }catch(SQLException se2){
                    }
                    try{
                        if(conn!=null)
                            conn.close();
                    }catch(SQLException se){
                        se.printStackTrace();
                    }
                }
                System.out.println("\n\n- MySQL Connection Close");

                break;
        }*/


       /* if(readMessage.getDatatype().equals("SignUp")){ // 입력받은 데이터 타입이 회원가입인 경우

        }
        else if(splitsArr[0].equals("Login")){
            Connection conn = null;
            Statement stmt = null; // 데이터를 전송하는 객체
            try{
                Class.forName(JDBC_DRIVER);
                conn = DriverManager.getConnection(DB_URL,USERNAME,PASSWORD); // 데이터 베이스 연결
                System.out.println("\n- MySQL Connection");
                stmt = conn.createStatement();

                // member란 테이블에서 gamepw, gameid 컬럼을 찾는다.
                String sql = "SELECT gamepw, gameid FROM member where gameid = ?"; // sql문을 만들어준다.
                PreparedStatement pstmt = null;
                ResultSet rs = null;
                try {
                    pstmt = conn.prepareStatement(sql);
                    pstmt.setString(1, splitsArr[1]);
                    rs = pstmt.executeQuery();

                    rs.last();
                    System.out.println("크기 : " + rs.getRow());

                    if(rs.getRow() == 0){ // 아이디 검색결과 없다. 가입가능
                        System.out.println("검색결과 없습니다.");
                        System.out.println(splitsArr[1] + "은 회원목록에 없는 아이디입니다.");
                        com.teamnova.server.SendingObject user = new com.teamnova.server.SendingObject("Login", "1");

                        ctx.writeAndFlush(user.Type + "/" + user.result);

                    }else {
                        System.out.println("검색결과 있습니다.");
                        System.out.println(splitsArr[1] + "은 회원목록에 있는 아이디입니다.");
                        ////////////////////////
                        com.teamnova.server.SendingObject user = new com.teamnova.server.SendingObject("Login", "2");

                        ByteArrayOutputStream baos = new ByteArrayOutputStream();
                        ObjectOutputStream oos = new ObjectOutputStream(baos);
                        oos.writeObject(user);
                        oos.close();
                        baos.close();
                        byte[] wrt = baos.toByteArray();
                        ByteBuf writebuf = Unpooled.directBuffer();
                        writebuf.writeBytes(wrt);

                        ChannelFuture cf = ctx.writeAndFlush(user.Type + "/" + user.result);
                        //ChannelFuture cf = ctx.writeAndFlush(writebuf);
                        cf.addListener(new ChannelFutureListener() {
                            @Override
                            public void operationComplete(ChannelFuture future) throws Exception {
                                if(future.isSuccess()) {
                                    System.out.println("서버에서 전송 성공");
                                }
                                else {
                                    System.out.println("서버에서 전송  실패");
                                }
                            }
                        });
                    }
                } catch (SQLException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                } finally {
                    try {
                        if (pstmt != null && !pstmt.isClosed())
                            pstmt.close();
                    } catch (SQLException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                }

                rs.close();
                stmt.close();
                conn.close();
            }catch(SQLException se1){
                se1.printStackTrace();
            }catch(Exception ex){
                ex.printStackTrace();
            }finally{
                try{
                    if(stmt!=null)
                        stmt.close();
                }catch(SQLException se2){
                }
                try{
                    if(conn!=null)
                        conn.close();
                }catch(SQLException se){
                    se.printStackTrace();
                }
            }
            System.out.println("\n\n- MySQL Connection Close");
        }*/


    }


    // 채널 읽는 것을 완료했을 때 동작할 코드를 정의 합니다.
    // channelRead의 마지막 호출에서 현재 일괄 처리의 마지막 메세지를 처리했음을 핸들러에게 통보
    public void channelReadComplete(ChannelHandlerContext ctx) throws Exception {
        // 대기중인 메시지를 원격 피어로 플러시하고 채널을 닫음
        //ctx.writeAndFlush(Unpooled.EMPTY_BUFFER).addListener(ChannelFutureListener.CLOSE);
        ctx.flush();
    }

    // 예외가 발생할 때 동작할 코드를 정의 합니다.
    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause)
            throws Exception {
        cause.printStackTrace(); // 쌓여있는 트레이스를 출력합니다.
        ctx.close(); // 컨텍스트를 종료시킵니다.
    }
}
