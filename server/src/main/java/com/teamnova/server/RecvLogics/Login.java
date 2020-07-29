package com.teamnova.server.RecvLogics;



import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.MainServer;
import com.teamnova.server.Props;
import com.teamnova.server.object.LoginData;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.Type;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelFutureListener;
import io.netty.channel.ChannelHandlerContext;


public class Login {
    private final LoginData loginPacket;
    private Props DB;
    private ChannelHandlerContext socket;
    private int IDCode;


    public Login(ChannelHandlerContext sock, LoginData loginData, int IDCode) {
        this.socket = sock;
        this.loginPacket = loginData;
        this.IDCode = IDCode;
        DB = MainServer.props;
    }

    public boolean GameLogin(){
        boolean LoginSuccess = false;
        Connection conn = null;
        Statement stmt = null; // 데이터를 전송하는 객체
        try{
            System.out.println("DB :  " + DB.dbdriver);
            Class.forName(DB.dbdriver);
            conn = DriverManager.getConnection(DB.database,DB.dbuser,DB.dbpassword); // 데이터 베이스 연결
            System.out.println("\n- MySQL Connection");
            stmt = conn.createStatement();

            // member란 테이블에서 gamepw, gameid 컬럼을 찾는다.
            String sql = "SELECT gamepw, gameid, Potion100, Potion50, Gold FROM member where gameid = ?"; // sql문을 만들어준다.
            PreparedStatement pstmt = null;
            ResultSet rs = null;
            try {
                pstmt = conn.prepareStatement(sql);
                pstmt.setString(1, loginPacket.ID());
                rs = pstmt.executeQuery();
                rs.last(); // 조회한 라인의 마지막 라인으로 이동
                //System.out.println("크기 : " + rs.getRow());

                //========================================================================
                // 로그인 실패 - 가입된 아이디 없음
                //========================================================================
                if(rs.getRow() == 0){ // 아이디 검색결과 없다. 로그인 실패
                    System.out.println("\n\n*------| ID Searching Result");
                    System.out.println("*------| [ "+loginPacket.ID() + " ] is not out member.");
                    System.out.println("*------| Login Failed!\n\n");

                    // 데이터를 보낼 플랫버퍼
                    FlatBufferBuilder flatBufferBuilder = new FlatBufferBuilder(8192);
                    int lg = LoginData.createLoginData(flatBufferBuilder, 0, 0, false, IDCode, 0, 0, 0); // 로그인 실패
                    int pk = Packet.createPacket(flatBufferBuilder, Type.LoginData, lg);
                    flatBufferBuilder.finish(pk);
                    byte[] sendbyte = flatBufferBuilder.sizedByteArray();
                    socket.channel().writeAndFlush(sendbyte);



                    //========================================================================
                    // 로그인 성공
                    //========================================================================
                }else {
                    System.out.println("\n\n*------| ID Searching Result");
                    System.out.println("*------| [ "+loginPacket.ID() + " ] is our member");
                    String dbPassword = rs.getString("gamepw");
                    if(dbPassword.equals(loginPacket.Password())){ //비밀번호가 일치
                        System.out.println("*------| Login Success!");
                        System.out.println("*------| Login ID " + loginPacket.ID() +"(" + IDCode + ") [Gold: "+ rs.getInt("Gold") + " 100% : " + rs.getInt("Potion100") + "50% : " + rs.getInt("Potion50")+"  \n\n");
                        // 클라이언트에 전송할 메세지 객체를 만들어준다.

                        // 데이터를 보낼 플랫버퍼
                        FlatBufferBuilder flatBufferBuilder = new FlatBufferBuilder(8192);
                        int lg = LoginData.createLoginData(flatBufferBuilder, 0, 0, true, IDCode, rs.getInt("Potion100"), rs.getInt("Potion50"), rs.getInt("Gold")); // 로그인 성공
                        int pk = Packet.createPacket(flatBufferBuilder, Type.LoginData, lg);
                        flatBufferBuilder.finish(pk);
                        byte[] sendbyte = flatBufferBuilder.sizedByteArray();
                        ChannelFuture cf = socket.writeAndFlush(sendbyte);
                        cf.addListener(new ChannelFutureListener() {
                            public void operationComplete(ChannelFuture future) throws Exception {
                                if(future.isSuccess()) {
                                    System.out.println("*------| Server->Cli : Send Successful");
                                }
                                else {
                                    System.out.println("*------| Server->Cli : Send Failed");
                                }
                            }
                        });

                        LoginSuccess = true;
                        //========================================================================
                        // 로그인 실패 - 비밀번호 불일치
                        //========================================================================
                    }else { // 비밀번호 불일치
                        System.out.println("*------| Password incorrect! Login Failed!\n\n");
                        // 클라이언트에 전송할 메세지 객체를 만들어준다.
                        // 데이터를 보낼 플랫버퍼
                        FlatBufferBuilder flatBufferBuilder = new FlatBufferBuilder(8192);
                        int lg = LoginData.createLoginData(flatBufferBuilder, 0, 0, false, IDCode,0, 0, 0); // 로그인 실패
                        int pk = Packet.createPacket(flatBufferBuilder, Type.LoginData, lg);
                        flatBufferBuilder.finish(pk);
                        byte[] sendbyte = flatBufferBuilder.sizedByteArray();
                        socket.writeAndFlush(sendbyte);
                    }

                }
            } catch (SQLException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } finally {
                try {
                    if (pstmt != null)
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
        return LoginSuccess;
    }

    public int[] GetInventory(String ID){
        Connection conn = null;
        Statement stmt = null; // 데이터를 전송하는 객체

        int[] result = new int[4];
        try{
            System.out.println("DB :  " + DB.dbdriver);
            Class.forName(DB.dbdriver);
            conn = DriverManager.getConnection(DB.database,DB.dbuser,DB.dbpassword); // 데이터 베이스 연결
            System.out.println("\n- MySQL Connection");
            stmt = conn.createStatement();

            // member란 테이블에서 gamepw, gameid 컬럼을 찾는다.
            String sql = "SELECT Point, Potion100, Potion50, Gold FROM member where gameid = ?"; // sql문을 만들어준다.
            PreparedStatement pstmt = null;
            ResultSet rs = null;
            try {
                pstmt = conn.prepareStatement(sql);
                pstmt.setString(1, ID);
                rs = pstmt.executeQuery();
                rs.last(); // 조회한 라인의 마지막 라인으로 이동
                //System.out.println("크기 : " + rs.getRow());

                result[0] = rs.getInt("Point"); // 0번 : 최대포인트
                result[1] = rs.getInt("Potion100"); // 1 : 100%회복 포션
                result[2] = rs.getInt("Potion50"); // 2: 50%회복포션
                result[3] = rs.getInt("Gold"); // 3: 골드


            } catch (SQLException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } finally {
                try {
                    if (pstmt != null)
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
        return result;
    }
}
