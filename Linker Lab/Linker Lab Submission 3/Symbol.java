package main;

public class Symbol {

	private String symbol;
	private int absAddress;
	private int relAddress;
	private int modNum;
	private boolean hasErrorMessage;
	private boolean hasRelAddress;
	private boolean isUsed;
	private boolean isUsedInModule;
	private String ErrorMessage;
	
	public Symbol(String symbol){
		this.symbol = symbol;
		this.hasErrorMessage = false;
		this.hasRelAddress = false;
		this.isUsed = false;
		this.isUsedInModule = false;
	}
	
	public boolean isUsed() {
		return isUsed;
	}

	public void setUsed(boolean isUsed) {
		this.isUsed = isUsed;
	}

	public boolean isUsedInModule() {
		return isUsedInModule;
	}

	public void setUsedInModule(boolean isUsedInModule) {
		this.isUsedInModule = isUsedInModule;
	}

	public boolean hasRelAddress() {
		return hasRelAddress;
	}

	public void hasRelAddress(boolean hasRelAddress) {
		this.hasRelAddress = hasRelAddress;
	}
	
	public void setRelAddress(int relAddress){
		this.relAddress = relAddress;
	}
	
	public int getRelAddress(){
		return relAddress;
	}
	
	public String getSymbol() {
		return symbol;
	}
	
	public int getAbsAddress() {
		return absAddress;
	}

	public void setAbsAddress(int absAddress) {
		this.absAddress = absAddress;
	}

	public boolean hasErrorMessage() {
		return hasErrorMessage;
	}

	public void setHasErrorMessage(boolean hasErrorMessage) {
		this.hasErrorMessage = hasErrorMessage;
	}
	
	public String getErrorMessage() {
		return ErrorMessage;
	}

	public void setErrorMessage(String errorMessage) {
		ErrorMessage = errorMessage;
	}
	
	public int getModNum() {
		return modNum;
	}

	public void setModNum(int modNum) {
		this.modNum = modNum;
	}

	
	@Override
	public boolean equals(Object o){
		if(!(o instanceof Symbol))
			return false;
		else{
			
			if(((Symbol) o).getSymbol().equals(this.symbol))
				return true;
			else
				return false;
		}
			
	}
	
	@Override
	public int hashCode(){
		return this.symbol.hashCode();
	}
	
	@Override
	public String toString(){
		return symbol;
	}

}
