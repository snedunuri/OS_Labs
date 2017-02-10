package main;

import java.util.ArrayList;
import java.util.LinkedList;

public class Module {
	
	private int modNum;
	private int baseAddress;
	private int modSize;
	private ArrayList<Symbol> useList;
	private ArrayList<Symbol> defList;
	private LinkedList<String> warningMessages;
	
	public Module(int modNum, int baseAdress, int modSize,
				  ArrayList<Symbol> useList, ArrayList<Symbol> defList){
		this.modNum = modNum;
		this.baseAddress = baseAdress;
		this.modSize = modSize;
		this.useList = useList;
		this.defList = defList;
		this.warningMessages = new LinkedList<String>();
	}
	
	public int getModSize() {
		return modSize;
	}

	public void setModSize(int modSize) {
		this.modSize = modSize;
	}

	public int getModNum() {
		return modNum;
	}

	public int getBaseAddress() {
		return baseAddress;
	}

	public ArrayList<Symbol> getUseList() {
		return useList;
	}
	
	
	public ArrayList<Symbol> getDefList() {
		return defList;
	}

	public void addWarningMessage(String msg){
		this.warningMessages.add(msg);
	}
	
	public String getWarningMessage(int index){
		return this.warningMessages.get(index);
	}
	
	public LinkedList<String> getAllWarningMessages(){
		return this.warningMessages;
	}

}
