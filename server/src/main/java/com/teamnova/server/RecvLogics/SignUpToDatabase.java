/*
package com.teamnova.server.RecvLogics;

import com.google.flatbuffers.FlatBufferBuilder;

import java.net.Socket;
import java.net.SocketException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import flatserver.MainServer;
import flatserver.Props;
import flatserver.object.Packet;
import flatserver.object.SignUpData;
import flatserver.object.Type;

// 회원가입여부를 판단하고 회원가입을 시켜주거나 거절하는 클래스이다.
public class SignUpToDatabase {
    private Props DB;

    public SignUpToDatabase(Socket sock, SignUpData signUpData){
        DB = MainServer.props;
        Connection conn = null;
        Statement stmt = null; // 데이터를 전송하는 객체
        try{
            System.out.println("DB :  " + DB.dbdriver);
            Class.forName(DB.dbdriver);
            conn = DriverManager.getConnection(DB.database,DB.dbuser,DB.dbpassword); // 데이터 베이스 연결
            System.out.println("\n- MySQL Connection");
            stmt = conn.createStatement();

            // member란 테이블에서 gamepw, gameid 컬럼을 찾는다.
            String sql = "SELECT gamepw, gameid FROM member where gameid = ?"; // sql문을 만들어준다.
            PreparedStatement pstmt = null;
            ResultSet rs = null;
            try {
                pstmt = conn.prepareStatement(sql);
                pstmt.setString(1, signUpData.ID());
                rs = pstmt.executeQuery();
                rs.last(); // 조회한 라인의 마지막 라인으로 이동
                //System.out.println("크기 : " + rs.getRow());

                if(rs.getRow() == 0){ // 아이디 검색결과 없다. 가입가능
                    System.out.println("\n\n*------| 아이디 검색 결과");
                    System.out.println("*------| [ "+signUpData.ID() + " ] 은 회원목록에 없는 아이디입니다.");
                    System.out.println("*------| 회원가입을 진행합니다.\n\n");
                    // 회원가입 처리부분
                    String insertsql = "insert into member (gamepw, gameid) value (?, ?)";
                    //PreparedStatement pstmt = null;
                    try{
                        pstmt = conn.prepareStatement(insertsql);
                        pstmt.setString(1, signUpData.Password());
                        pstmt.setString(2, signUpData.ID());
                        pstmt.executeUpdate();
                        System.out.println("*------| 회원가입 성공!");

                    }catch (SQLException e){
                        e.printStackTrace();
                    }finally {
                        try {
                            if(pstmt != null)
                                pstmt.close();
                        }catch (SQLException e){
                            e.printStackTrace();
                        }
                    }

                    FlatBufferBuilder flatBufferBuilder1 = new FlatBufferBuilder(1024); // 패킷을 담을 플랫버퍼를 만든다.
                    int sg =  SignUpData.createSignUpData(flatBufferBuilder1, 0, 0, true); // 회원가입 데이터를 담는다.
                    int pk = Packet.createPacket(flatBufferBuilder1, Type.SignUpData, sg); // 회원가입데이터를 패킷에 담는다.

                    flatBufferBuilder1.finish(pk); // 플랫버퍼에 담는다.
                    byte[] sendbyte = flatBufferBuilder1.sizedByteArray(); // 바이트 배열로 변환
                    sock.getOutputStream().write(sendbyte); // 소켓에 보내준다.



                }else {
                    System.out.println("\n\n*------| 아이디 검색 결과");
                    System.out.println("*------| [ "+signUpData.ID() + " ] 은 회원목록에 있는 아이디입니다.");
                    System.out.println("*------| 회원가입을 할 수 없는 아이디 입니다.\n\n");
                    ////////////////////////
                    FlatBufferBuilder flatBufferBuilder1 = new FlatBufferBuilder(1024);
                    int sg =  SignUpData.createSignUpData(flatBufferBuilder1, 0, 0, false);
                    int pk = Packet.createPacket(flatBufferBuilder1, Type.SignUpData, sg);

                    flatBufferBuilder1.finish(pk);
                    byte[] sendbyte = flatBufferBuilder1.sizedByteArray();
                    sock.getOutputStream().write(sendbyte);
                }
            }catch (SocketException e){
                System.out.println("Socket Execption!");
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

    }

}
*/
