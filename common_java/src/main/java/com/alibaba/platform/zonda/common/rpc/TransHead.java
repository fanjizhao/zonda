package com.alibaba.platform.zonda.common.rpc;

import javolution.io.Struct.Signed32;

public class TransHead extends HeadBase {
	public final Signed32 version = new Signed32();
	public final Signed32 rpcHeadLen = new Signed32();
	public final Signed32 rpcMsgLen = new Signed32();
	public final Signed64 callId = new Signed64();

	public static void main(String[] args) {
		byte[] b = new byte[12];
		b[2] = 0x11;
		TransHead a = new TransHead();
		try {
			System.out.println(a.toString());
			a.deserialize(b);
			System.out.println(a.version);
		} catch (Exception e) {
			e.printStackTrace();
		}

	}
}