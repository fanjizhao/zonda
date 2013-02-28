package com.alibaba.platform.zonda.common.rpc;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import javolution.io.Struct;

public class HeadBase extends Struct 
{
	public HeadBase() 
	{

	}

	public boolean isPacked() 
	{
		return false;
	}

	public void deserialize(String instr) 
	{
		this.deserialize(instr.getBytes());
	}

	public void deserialize(byte[] buff) 
	{
		this.setByteBuffer(ByteBuffer.wrap(buff), 0);
	}

	public void deserialize(byte[] buff, int offset, int len) throws Exception
	{
		read(new ByteArrayInputStream(buff, offset, len));
	}
}