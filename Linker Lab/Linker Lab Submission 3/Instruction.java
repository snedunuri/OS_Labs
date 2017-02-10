package main;

public class Instruction {

	private int instrNum;
	private int address;
	private int modNum;
	private int useIndex;
	private char instrType;
	private boolean hasErrorMessage;
	private String errorMessage;
	
	public Instruction(int instrNum, int address, int modNum, char instrType){
		this.instrNum = instrNum;
		this.address = address;
		this.modNum = modNum;
		this.instrType = instrType;
	}

	public boolean hasErrorMessage() {
		return hasErrorMessage;
	}

	public void setHasErrorMessage(boolean hasErrorMessage) {
		this.hasErrorMessage = hasErrorMessage;
	}

	public String getErrorMessage() {
		return errorMessage;
	}

	public void setErrorMessage(String errorMessage) {
		this.errorMessage = errorMessage;
	}

	public int getInstrNum() {
		return instrNum;
	}
	
	public int getModNum() {
		return modNum;
	}

	public char getInstrType() {
		return instrType;
	}

	public void setAddress(int address){
		this.address = address;
	}

	public int getAddress() {
		return address;
	}
	
	public void setUseIndex(int index){
		this.useIndex = index;
	}
	
	public int getUseIndex(){
		return useIndex;
	}
	
}
