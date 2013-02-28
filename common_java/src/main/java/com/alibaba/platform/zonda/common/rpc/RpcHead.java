package com.alibaba.platform.zonda.common.rpc;

import java.nio.ByteBuffer;

public class RpcHead extends HeadBase {
	public enum RpcResCode {
		SUCCESS(0), TIMEOUT(1), SEND_FAIL(2);
		private int value;

		private RpcResCode(int value) {
			this.value = value;
		}

		public int getValue() {
			return value;
		}
	}

	public RpcHead() {
	}

	public final Signed32 msgType = new Signed32();
	public final ServiceAddr responser = inner(new ServiceAddr()); 
	public final ServiceAddr invoker = inner(new ServiceAddr()); 
	public final Signed64 transactionId = new Signed64();
	public final Signed64 transParam = new Signed64();
	public final Signed64 callId = new Signed64();
	public final Unsigned16 bizHandlerId = new Unsigned16();
	public final Signed16 resCode = new Signed16();

	public String toPtrString() {
		return "RpcHead [msgType=" + msgType + ", responser=" + responser.toPtrString() + ", invoker="
				+ invoker.toPtrString() + ", transactionId=" + transactionId + ", transParam=" + transParam
				+ ", callId=" + callId + ", bizHandlerId=" + bizHandlerId + ", resCode=" + resCode + "]";
	}

	public static void main(String[] args) {
		RpcHead head = new RpcHead();
		ByteBuffer bb = ByteBuffer.allocate(1024);
		bb.put(head.getByteBuffer());
		System.out.println("pos:" + bb.position());
		System.out.println("limit:" + bb.limit());
		
		System.out.println(head.size() );
	}
}
