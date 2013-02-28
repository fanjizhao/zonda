/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-3
 * DateUtil: DateUtil.java 12-2-3 darwin $
 *
 * Copyright 2008 Alibaba.com Croporation Limited.
 * All rights reserved.
 *
 * This software is the confidential and proprietary information of
 * Alibaba Company. ("Confidential Information").  You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered into
 * with Alibaba.com.
 */
package com.alibaba.zonda.logger.server.util;

import java.util.Calendar;
import java.util.Date;

/**
 * User: darwin
 * Date: 12-2-3
 * Time: 11:43 am
 */
public class DateUtil {

    public static Date parseDate(String separator, String s) {
        String[] tokens = s.split(separator);
        Calendar cal = Calendar.getInstance();
        if (tokens.length == 4)
            cal.set(Integer.valueOf(tokens[0]), Integer.valueOf(tokens[1])-1,Integer.valueOf(tokens[2]),
                    Integer.valueOf(tokens[3]),0,0);
        else
            cal.set(Integer.valueOf(tokens[0]), Integer.valueOf(tokens[1])-1,Integer.valueOf(tokens[2]),
                    Integer.valueOf(tokens[3]),Integer.valueOf(tokens[4]),0);
        return cal.getTime();

    }
    public static String preDate(char separator){
        Calendar cal=Calendar.getInstance();
        cal.add(Calendar.DAY_OF_MONTH, -1);
        StringBuilder sb=new StringBuilder();
        sb.append(cal.get(Calendar.YEAR));
        sb.append(separator);
        sb.append(padding(cal.get(Calendar.MONTH)+1));
        sb.append(separator);
        sb.append(padding(cal.get(Calendar.DAY_OF_MONTH)));
        return sb.toString();
    }
    public static String getTimeStampInHour(char separator, Date date) {
        Calendar cal = Calendar.getInstance();
        if (date != null)
            cal.setTime(date);
        StringBuilder sb = new StringBuilder();
        sb.append(cal.get(Calendar.YEAR));
        sb.append(padding(cal.get(Calendar.MONTH) + 1));
        sb.append(padding(cal.get(Calendar.DAY_OF_MONTH)));
        sb.append(separator);
        sb.append(padding(cal.get(Calendar.HOUR_OF_DAY)));
        return sb.toString();
    }
    public static String getTimeStampInMin(char separator, Date date) {
        Calendar cal = Calendar.getInstance();
        if (date != null)
            cal.setTime(date);
        StringBuilder sb = new StringBuilder();
        sb.append(cal.get(Calendar.YEAR));
        sb.append(separator);
        sb.append(padding(cal.get(Calendar.MONTH)+1));
        sb.append(separator);
        sb.append(padding(cal.get(Calendar.DAY_OF_MONTH)));
        sb.append(separator);
        sb.append(padding(cal.get(Calendar.HOUR_OF_DAY)));
        sb.append(separator);
        sb.append(padding(cal.get(Calendar.MINUTE)));
        return sb.toString();
    }

    public static String getTimeStampInSec(char separator, Date date) {
        Calendar cal = Calendar.getInstance();
        if (date != null)
            cal.setTime(date);
        StringBuilder sb = new StringBuilder();
        sb.append(cal.get(Calendar.YEAR));
        sb.append(separator);
        sb.append(padding(cal.get(Calendar.MONTH) + 1));
        sb.append(separator);
        sb.append(padding(cal.get(Calendar.DAY_OF_MONTH)));
        sb.append(separator);
        sb.append(padding(cal.get(Calendar.HOUR_OF_DAY)));
        sb.append(separator);
        sb.append(padding(cal.get(Calendar.MINUTE)));
        sb.append(separator);
        sb.append(padding(cal.get(Calendar.SECOND)));
        return sb.toString();
    }

    public static String getTimeStampInMin(Date date) {
        Calendar cal=Calendar.getInstance();
        if (date != null)
            cal.setTime(date);
        StringBuilder sb = new StringBuilder();
        sb.append(cal.get(Calendar.YEAR));
        sb.append(padding(cal.get(Calendar.MONTH)+1));
        sb.append(padding(cal.get(Calendar.DAY_OF_MONTH)));
        sb.append("/");
        sb.append(padding(cal.get(Calendar.HOUR_OF_DAY)));
        sb.append("/");
        sb.append(padding(cal.get(Calendar.MINUTE)));
        return sb.toString();
    }

    public static String padding(int i) {
        if (i == 0)
            return "00";
        else
        if (i < 10)
            return "0" + i;
        else
            return "" + i;
    }
    public static Date beginOfDate(Date date) {
        Calendar cal = Calendar.getInstance();
        cal.setTime(date);
        cal.set(Calendar.HOUR_OF_DAY, 0);
        cal.set(Calendar.MINUTE,0);
        cal.set(Calendar.SECOND, 0);
        cal.set(Calendar.MILLISECOND,0);
        return cal.getTime();
    }
    public static Date nextDate(Date date) {
        Calendar cal = Calendar.getInstance();
        cal.setTime(date);
        cal.add(Calendar.DAY_OF_MONTH,1);
        return cal.getTime();
    }
    public static Date nextHour(Date date) {
        Calendar cal = Calendar.getInstance();
        cal.setTime(date);
        cal.add(Calendar.HOUR_OF_DAY, 1);
        return cal.getTime();
    }

}
