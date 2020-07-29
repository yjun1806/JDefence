package com.teamnova.server;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

public class Props {
    public String dbpassword;
    public String database;
    public String dbuser;
    public String dbdriver;
    public String port;

    Props(){
        try {
            String propFile = "configuration.properties";
            // 프로퍼티 객체 생성
            Properties props = new Properties();
            // 프로퍼티 파일 스트림에 담기
            InputStream inputStream = getClass().getClassLoader().getResourceAsStream(propFile);
            // 프로퍼티 파일 로딩
            props.load(inputStream);

            port = props.getProperty("tcpPort");
            dbpassword = props.getProperty("PASSWORD");
            database = props.getProperty("DB_URL");
            dbuser = props.getProperty("USERNAME");
            dbdriver = props.getProperty("JDBC_DRIVER");

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


}
