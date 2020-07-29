package com.teamnova.server.RecvLogics;



import com.google.flatbuffers.FlatBufferBuilder;
import com.teamnova.server.MainServer;
import com.teamnova.server.Props;
import com.teamnova.server.object.Packet;
import com.teamnova.server.object.RankUser;
import com.teamnova.server.object.TopRanking;
import com.teamnova.server.object.Type;

import java.net.SocketException;
import java.nio.ByteBuffer;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;


import io.netty.channel.ChannelId;

import static com.teamnova.server.MainServer.HT_ConnectedAllUserInfo;


public class SaveResultDB {
    private Props DB;
    public SaveResultDB(){
        DB = MainServer.props;
    }

    public SaveResultDB(ChannelId ID, int Point){
        DB = MainServer.props;
        Connection conn = null;
        Statement stmt = null; // 데이터를 전송하는 객체

        try{
            System.out.println("DB :  " + DB.dbdriver);
            Class.forName(DB.dbdriver);
            conn = DriverManager.getConnection(DB.database,DB.dbuser,DB.dbpassword); // 데이터 베이스 연결
            System.out.println("\n- MySQL Connection");
            stmt = conn.createStatement();

            String Userid = HT_ConnectedAllUserInfo.get(ID).UserID;
            // member란 테이블에서 gamepw, gameid 컬럼을 찾는다.
            String sql = "SELECT Point FROM member where gameid = ?"; // sql문을 만들어준다.
            PreparedStatement pstmt = null;
            ResultSet rs = null;
            try {
                pstmt = conn.prepareStatement(sql);
                pstmt.setString(1, Userid);
                rs = pstmt.executeQuery();
                rs.last(); // 조회한 라인의 마지막 라인으로 이동
                //System.out.println("크기 : " + rs.getRow());

                if(rs.getInt("Point") < Point){
                    StringBuilder sb = new StringBuilder();
                    String sql2 = sb.append("update member set")
                            .append(" Point = ")
                            .append(Point)
                            .append(" where gameid = ")
                            .append("'" + Userid + "'")
                            .append(";").toString();
                    pstmt.executeUpdate(sql2);

                    if(HT_ConnectedAllUserInfo.get(ID).MaxPoint < Point){
                        HT_ConnectedAllUserInfo.get(ID).MaxPoint = Point;
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
    }

    public void SendTopRanking(ChannelId userid){
        Connection conn = null;
        Statement stmt = null; // 데이터를 전송하는 객체

        try{
            System.out.println("DB :  " + DB.dbdriver);
            Class.forName(DB.dbdriver);
            conn = DriverManager.getConnection(DB.database,DB.dbuser,DB.dbpassword); // 데이터 베이스 연결
            System.out.println("\n- MySQL Connection");
            stmt = conn.createStatement();

            // member라는 테이블에서 Point컬럼을 기준으로 오름차순 정렬해서 10개의 데이터만 가져온다.
            String sql = "SELECT * FROM member order by Point desc"; // sql문을 만들어준다.
            PreparedStatement pstmt = null;
            ResultSet rs = null;
            try {
                pstmt = conn.prepareStatement(sql);
                pstmt.setMaxRows(10); // 가져오는 데이터 개수 10로 제한
                rs = pstmt.executeQuery();

                FlatBufferBuilder fb = new FlatBufferBuilder(8192);
                int rk[] = new int[10];
                int i=0;
                while (rs.next() && i < 10){
                    System.out.println("가져온 랭킹 데이터 : " + rs.getString("gameid") + " / " + rs.getInt("Point"));
                    rk[i] = RankUser.createRankUser(fb,
                            fb.createString(rs.getString("gameid")),
                            rs.getInt("Point"));
                    i++;
                }
                int tp = TopRanking.createRanklistVector(fb, rk);
                TopRanking.startTopRanking(fb);
                TopRanking.addRanklist(fb, tp);
                int tr = TopRanking.endTopRanking(fb);
                int pk = Packet.createPacket(fb, Type.TopRanking, tr);
                fb.finish(pk);
                byte[] bytes = fb.sizedByteArray();
                HT_ConnectedAllUserInfo.get(userid).socket.writeAndFlush(bytes); //TODO: 왜 0이가지?

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

    public void SaveItemToDB(ChannelId userid){
        Connection conn = null;
        Statement stmt = null; // 데이터를 전송하는 객체

        try{
            System.out.println("DB :  " + DB.dbdriver);
            Class.forName(DB.dbdriver);
            conn = DriverManager.getConnection(DB.database,DB.dbuser,DB.dbpassword); // 데이터 베이스 연결
            System.out.println("\n- MySQL Connection");
            stmt = conn.createStatement();

            String FindID = HT_ConnectedAllUserInfo.get(userid).UserID;
            int Potion100 = HT_ConnectedAllUserInfo.get(userid).Potion100;
            int Potion50 = HT_ConnectedAllUserInfo.get(userid).Potion50;
            int Gold = HT_ConnectedAllUserInfo.get(userid).Gold;
            // member란 테이블에서 gamepw, gameid 컬럼을 찾는다.
            String sql = "SELECT Potion100, Potion50, Gold FROM member where gameid = ?"; // sql문을 만들어준다.
            PreparedStatement pstmt = null;
            try {
                pstmt = conn.prepareStatement(sql);
                pstmt.setString(1, FindID);


                    StringBuilder sb = new StringBuilder();
                    String sql2 = sb.append("update member set")
                            .append(" Potion100 = ")
                            .append(Potion100)
                            .append(" ,Potion50 = ")
                            .append(Potion50)
                            .append(" ,Gold = ")
                            .append(Gold)
                            .append(" where gameid = ")
                            .append("'" + FindID + "'")
                            .append(";").toString();
                    pstmt.executeUpdate(sql2);



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

            //rs.close();
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
