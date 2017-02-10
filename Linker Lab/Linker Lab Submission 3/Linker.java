package main;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Scanner;
import java.util.TreeMap;


public class Linker {
	
	private File input;
	private Scanner scanner;
	private String instr;
	private boolean isFirstPass;
	private String[] instrLineArray;
	private TreeMap<String, Symbol> symbolTable;
	private TreeMap<Integer, Instruction> memMap;
	private ArrayList<Module> modList;
	private ArrayList<Symbol> curDefList;
	private ArrayList<Symbol> curUseList;
	
	private static final int DEF_LIMIT = 16;
	private static final int USE_LIMIT = 16;
	private static final int SYM_LEN_LIMIT = 16;
	private static final int MACHINE_SIZE = 512;
	
	private int prevLineCounter = 1;
	private int prevCharOffset = 0;
	private int lineCounter = 1;
	private int charOffset = 0;
	private int modCounter = 1;
	private int modBaseAddrCounter = 0;
	private int instrCounter = 0;
	private int instrTracker = 0;
	
	//error messages that are printed when a parsing error occurs
	private static final String[] parseErrorMsg = { "NUM_EXPECTED",
													"SYM_EXPECTED",
													"ADDR_EXPECTED",
													"SYM_TOLONG",
													"TO_MANY_DEF_IN_MODULE",
													"TO_MANY_USE_IN_MODULE",
													"TO_MANY_INSTR" };
	
	public Linker(File input){
		this.isFirstPass = true;
		this.input = input;
		this.symbolTable = new TreeMap<String, Symbol>();
		this.memMap = new TreeMap<Integer, Instruction>();
		this.modList = new ArrayList<Module>();
		this.curDefList = new ArrayList<Symbol>();
		this.curUseList = new ArrayList<Symbol>();
	}
	
	public void run() throws IOException{
		firstPass();
	}
	
	private void firstPass() throws IOException{
		//read in all the instructions into a string
		//and create an array of string split by a new line
		readInInstructions();
		
		//set line scanner
		setNextLine();
	    
		//a list containing all the modules and
		//their base addresses
	    createModulesList();
	    
	    //resolving the absolute addresses of all symbols
	    setSymbolAbsAddresses();
	    
	    //printing out the symbol table and any associated
	    //error messages
		printSymbolTable();
		
		//calling the second pass method to resolve relative and
		//external addresses
		secondPass();
	}
	
	private void readInInstructions() throws IOException{
		BufferedReader br = new BufferedReader(new FileReader(input));
	    StringBuilder sb = new StringBuilder();
	    String line = br.readLine();

	    while (line != null) {
	        sb.append(line);
	        sb.append(System.lineSeparator());
	        line = br.readLine();
	    }
	    
	    br.close();
	    instr = sb.toString();
	    
	    if(instr.endsWith("\n")){
	    	instr = instr.replace("\n", "\\$");
	    }
	    
	    //split instructions by line
	    //instrLineArray = instr.split("((?<=\\n)|(?=\\n))");
	    instrLineArray = instr.split("\\n");
	    
	    instrLineArray = instrLineArray[instrLineArray.length-1].split("((?<=\\$))");
	    
	    for(int i = 0; i < instrLineArray.length-1; i++){
	    	if(instrLineArray[i].contains("\\$") && !instrLineArray[i].equals("\\s")){
	    		instrLineArray[i] = instrLineArray[i].replace("\\$", "");
	    	}
	    }
	    
	    
	}
	
	private void printSymbolTable(){
		System.out.println("Symbol Table");
		
		for(Symbol symbol : symbolTable.values()){
			if(symbol.hasErrorMessage()){
				if(!symbol.getErrorMessage().equals("Undefined")){
				System.out.printf("%s=%d %s\n", symbol.getSymbol(), symbol.getAbsAddress(), 
								  symbol.getErrorMessage());
				}
			} else {
				System.out.printf("%s=%d\n", symbol.getSymbol(), symbol.getAbsAddress());
			}
		}
		System.out.println();
	}
	
	private void createModulesList(){
		
		do{
			readDefList();
			readUseList();
			
			//get number of instructions in a module
			int modSize = readInstrList();
			
			//create module
			Module mod = new Module(modCounter, modBaseAddrCounter, 
									modSize, curUseList, curDefList);
			modList.add(mod);
			
			if(!isFirstPass){
				ArrayList<Symbol> thisUseList = mod.getUseList();
				for(Symbol sym : thisUseList){
					if(!sym.isUsedInModule()){
						modList.get(modCounter-1).addWarningMessage(String.format("Warning: Module %d: %s appeared in the "
								+ "uselist but was not actually used", mod.getModNum(), sym.getSymbol()));
					}
				}
			}
			
			//check to see if address in definition exceeds module size
			if(isFirstPass)
				validateDefAddresses();

			//check to see if relative and absolute addresses 
			//are less than module size
			if(!isFirstPass)
				validateAddresses();
			
			//create new use list for next module
			curUseList = new ArrayList<Symbol>();
			curDefList = new ArrayList<Symbol>();
			
			//update module info
			modCounter++;
			modBaseAddrCounter = mod.getBaseAddress() + mod.getModSize();
			
		} while (hasNextLine());
		
	}
	
	private void validateDefAddresses(){
		Module mod = modList.get(modList.size()-1); //get the last module
		for(Symbol symbol : curDefList){
			if(symbol.getRelAddress() > mod.getModSize()){
				System.out.print(String.format("Warning: Module %d: %s to big %d (max=%d)"
											 + " assume zero relative\n", mod.getModNum(), symbol.getSymbol(),
											 symbol.getRelAddress(), mod.getModSize()-1));
				symbol.setRelAddress(0);
			}
		}
	}
	
	private void setSymbolAbsAddresses(){
		
		Iterator<Symbol> iter = symbolTable.values().iterator();
		
		while(iter.hasNext()){
			Symbol symbol = iter.next();
			
			int modNumOfSymbol = symbol.getModNum();
			if (modNumOfSymbol == 0) 
				modNumOfSymbol = 1;
			int modBaseAddr = modList.get(modNumOfSymbol-1).getBaseAddress();
			
			if(symbol.hasRelAddress()){
				symbol.setAbsAddress(modBaseAddr + symbol.getRelAddress());
			}
			else {
				symbol.hasRelAddress(false);
				symbol.setAbsAddress(0);
				symbol.setHasErrorMessage(true);
				symbol.setErrorMessage("Undefined");
			}
		}
	}
	
	private void readDefList(){
		
		String token = getNextToken();
		
		try{
			int numDefs = Integer.parseInt(token);
			
			if(numDefs > DEF_LIMIT){
				printErrorMessage(4);
				System.exit(-1);
			}
			
			for(int i = 0; i < numDefs; i++){
				readDefs();
			}
			
		} catch (NumberFormatException e){
			printErrorMessage(0);
			System.exit(-1);
		}
	}
	
	private void readDefs(){
		
		
		try {
			String symbolName = getNextToken();
			
			validateSymbol(symbolName);
			
			int relAddress = Integer.parseInt(getNextToken());
			
			if(!symbolTable.containsKey(symbolName)){
				Symbol symbol = new Symbol(symbolName);
				symbol.setRelAddress(relAddress);
				symbol.hasRelAddress(true);
				symbol.setModNum(modCounter);
				symbolTable.put(symbolName, symbol);
				curDefList.add(symbol);
			} else {
				Symbol symbolInTable = symbolTable.get(symbolName);
				
				if(symbolInTable.hasRelAddress()){
					symbolInTable.setHasErrorMessage(true);
					symbolInTable.setErrorMessage("Error: This variable is multiple times defined; "
												+ "first value used");
				} else {
					symbolInTable.setRelAddress(relAddress);
					symbolInTable.hasRelAddress(true);
					symbolInTable.setModNum(modCounter);
				}
				curDefList.add(symbolInTable);
			}
			
		} catch (NumberFormatException e){
			printErrorMessage(0);
			System.exit(-1);
		} catch (IllegalStateException e){
			printErrorMessage(1);
			System.exit(-1);
		}
	
	}
	
	private void readUseList(){
		
		String token = getNextToken();
		
		try{
			int numUses = Integer.parseInt(token);
			
			if(numUses > USE_LIMIT){
				printErrorMessage(5);
				System.exit(-1);
			}
			
			for(int i = 0; i < numUses; i++){
				readUses();
			}
			
		} catch (NumberFormatException e){
			printErrorMessage(0);
			System.exit(-1);
		}
	}
	
	private void readUses(){
		
		String symbolName = getNextToken();
		
		validateSymbol(symbolName);
		
		Symbol useSymbol = new Symbol(symbolName);
		
		if(symbolTable.containsKey(symbolName)){
			Symbol symbolInTable = symbolTable.get(symbolName);
			curUseList.add(symbolInTable);
		} else {
			useSymbol.setModNum(modCounter);
			symbolTable.put(symbolName, useSymbol);
			curUseList.add(useSymbol);
		}
	}
	
	private int readInstrList(){
		
		int numInstr = 0;
		try{
			String token = getNextToken();
			
			numInstr = Integer.parseInt(token);
			
			instrTracker += numInstr;
			
			if(instrTracker > MACHINE_SIZE){
				printErrorMessage(6);
				System.exit(-1);
			}
			
			if(isFirstPass){
				for(int i = 0; i < numInstr; i++){
					String instrType = getNextToken();
					String addressString = getNextToken();
					
					if(addressString.length() < 4 && (instrType.equals("R") || instrType.equals("E"))){
						printErrorMessage(2);
						System.exit(-1);
					}
				}
			} else {
				for(int i = 0; i < numInstr; i++){
					String instrType = getNextToken();
					String addressString = getNextToken();
					if(instrType.equals("R"))
						createRTypeInstruction(addressString);
					else if (instrType.equals("E"))
						createETypeInstruction(addressString);
					else
						createIOrATypeInstruction(addressString, instrType);
				}
				
				
			}
		} catch(NumberFormatException e){
			printErrorMessage(0);
			System.exit(-1);
		} catch(IllegalStateException e){
			printErrorMessage(2);
			System.exit(-1);
		}
		
		return numInstr;
	}
	
	private void validateSymbol(String token){
		
		if(token.length() > SYM_LEN_LIMIT){
			printErrorMessage(3);
			System.exit(-1);
		}
		
		if(!token.matches("[a-zA-Z][a-zA-Z0-9]*")){
			printErrorMessage(1);
			System.exit(-1);
		}
	}
	
	private boolean hasNextLine(){
		return (lineCounter >= instrLineArray.length) ? false : true;
	}
	
	private void setNextLine(){
		if(instrLineArray[lineCounter-1].isEmpty()){
			while(instrLineArray[lineCounter-1].isEmpty())
				lineCounter++;
		}
		this.scanner = new Scanner(instrLineArray[lineCounter-1]);
	}
	
	private String getNextToken(){
		String token = null;
		prevLineCounter = lineCounter;
		prevCharOffset = charOffset;
		
		if(scanner.hasNext())
			token = scanner.next();
		else{
			if(hasNextLine()){
				lineCounter++;
				setNextLine();
				token = scanner.next();
			}
			else {
				throw new IllegalStateException();
			}
		}
		
		String curLine = instrLineArray[lineCounter-1];
		if(!token.contains("\\$")){
			//setting the character offset in a given line
			if(prevCharOffset < curLine.length()){
				charOffset = curLine.indexOf(token, prevCharOffset) + 1;
			}
			else{
				charOffset = curLine.indexOf(token) + 1;
			}
		} else {
			token = token.replace("\\$", "");
			
			if(token.length() != 4)
				charOffset = curLine.indexOf(token) + token.length() + 1;
			else{
				if(prevCharOffset < curLine.length()){
					charOffset = curLine.indexOf(token, prevCharOffset) + 1;
				}
				else{
					charOffset = curLine.indexOf(token) + 1;
				}
			}
		}
		
		return token;
	}
	
	private void printErrorMessage(int index){
		if(charOffset == 0)
			charOffset = 1;
		System.out.printf("Parse Error line %d offset %d: %s\n", lineCounter, charOffset, parseErrorMsg[index]);
	}
	
	/****************************************************SECOND PASS************************************************/
	
	private void secondPass(){
		//set the number of passes flag
		isFirstPass = false;
		
		//reset counters and scanner
		reset();
		
		//a list containing all the modules and
		//their base addresses
	    createModulesList();
	    
	    //print out the memory map
	    printMemoryMap();
		
	    //print additional warning messages
	    printUnusedSymbolWarnings();
	}
	
	private void reset(){
		lineCounter = 1;
		charOffset = 0;
		modCounter = 1;
		modBaseAddrCounter = 0;
		instrCounter = 0;
		setNextLine();
	}
	
	private void createRTypeInstruction(String addressString){
		Instruction instr = null;
		if(addressString.contains("\\$")){
			addressString = addressString.replace("\\$", "");
		}
		if(addressString.length() == 4){
			int relAddress = Integer.parseInt(addressString);
			int absAddress = relAddress + modBaseAddrCounter;
			instr = new Instruction(instrCounter, absAddress, modCounter, 'R');
			instr.setHasErrorMessage(false);
		} else if (addressString.length() > 4){
			int absAddress = 9999;
			instr = new Instruction(instrCounter, absAddress, modCounter, 'R');
			instr.setHasErrorMessage(true);
			instr.setErrorMessage("Error: Illegal opcode; treated as 9999");
		}
		
		memMap.put(instrCounter, instr);
		instrCounter++;
	}
	
	private void createETypeInstruction(String addressString){
		Instruction instr;
		if(addressString.contains("\\$")){
			addressString = addressString.replace("\\$", "");
		}
		if(addressString.length() == 4){
			int address = Integer.parseInt(addressString);
			int useIndex = address % 1000;
			ArrayList<Symbol> useList = modList.get(modCounter-1).getUseList();
			
			if(useIndex >= useList.size()){
				instr = new Instruction(instrCounter, address, modCounter, 'E');
				instr.setHasErrorMessage(true);
				instr.setErrorMessage("Error: External address exceeds length of uselist; " +
									  "treated as immediate");
			} else { 
				Symbol symbol = useList.get(useIndex);
				if(symbol.hasErrorMessage() && symbol.getErrorMessage().equals("Undefined")){
					symbol.setUsed(true);
					symbol.setUsedInModule(true);
					instr = new Instruction(instrCounter, address - address % 1000, modCounter, 'E');
					instr.setHasErrorMessage(true);
					instr.setErrorMessage(String.format("Error: %s is not defined; zero used", symbol.getSymbol()));
				}
				else {
					symbol.setUsed(true);
					symbol.setUsedInModule(true);
					int absAddress = useList.get(useIndex).getAbsAddress();
					int finalAddress = Integer.parseInt(String.format("%s%03d", addressString.substring(0,1), absAddress));
					instr = new Instruction(instrCounter, finalAddress, modCounter, 'E');
					instr.setHasErrorMessage(false);
					instr.setUseIndex(useIndex);
				}
			}
		} else {
			int absAddress = 9999;
			instr = new Instruction(instrCounter, absAddress, modCounter, 'E');
			instr.setHasErrorMessage(true);
			instr.setErrorMessage("Error: Illegal opcode; treated as 9999");
		}
		
		memMap.put(instrCounter, instr);
		instrCounter++;
	}
	
	private void createIOrATypeInstruction(String addressString, String instrType){
		Instruction instr = null;
		if(addressString.contains("\\$")){
			addressString = addressString.replace("\\$", "");
		}
		if(addressString.length() == 4){
			int address = Integer.parseInt(addressString);
			instr = new Instruction(instrCounter, address, modCounter, instrType.charAt(0));
			instr.setHasErrorMessage(false);
		} else if (addressString.length() > 4 && instrType.equals("I")){
			int absAddress = 9999;
			instr = new Instruction(instrCounter, absAddress, modCounter, instrType.charAt(0));
			instr.setHasErrorMessage(true);
			instr.setErrorMessage("Error: Illegal immediate value; treated as 9999");
		} else if (addressString.length() > 4 && instrType.equals("A")){
			int absAddress = 9999;
			instr = new Instruction(instrCounter, absAddress, modCounter, instrType.charAt(0));
			instr.setHasErrorMessage(true);
			instr.setErrorMessage("Error: Illegal opcode; treated as 9999");
		} else if (addressString.length() < 4){
			int address = Integer.parseInt(addressString);
			instr = new Instruction(instrCounter, address, modCounter, instrType.charAt(0));
			instr.setHasErrorMessage(false);
		}
		
		memMap.put(instrCounter, instr);
		instrCounter++;
	}
	
	private void validateAddresses(){
		Module mod = modList.get(modList.size()-1); //get the last element
		for(Instruction instr : memMap.values()){
			int address = instr.getAddress() % 1000;
			if(instr.getInstrType() == 'A' && address > MACHINE_SIZE){
				instr.setHasErrorMessage(true);
				instr.setErrorMessage("Error: Absolute address exceeds machine size; zero used");
				instr.setAddress(instr.getAddress() - address);
			} else if(instr.getInstrType() == 'R' && address > mod.getBaseAddress() + mod.getModSize() && !instr.hasErrorMessage()){
				instr.setHasErrorMessage(true);
				instr.setErrorMessage("Error: Relative address exceeds module size; zero used");
				instr.setAddress(instr.getAddress() - address + mod.getBaseAddress());
			}
		}
	}
	
	
	private void printMemoryMap(){
		System.out.println("Memory Map");
		for(Instruction instr : memMap.values()){
			Module mod = modList.get(instr.getModNum()-1);
			if(instr.hasErrorMessage()){
				System.out.printf("%03d: %04d %s\n", instr.getInstrNum(), instr.getAddress(), 
								  instr.getErrorMessage());
			} else {
				System.out.printf("%03d: %04d\n", instr.getInstrNum(), instr.getAddress());
			}	
			
			boolean isLastInstrInMod = instr.getInstrNum()+1 == (mod.getBaseAddress() + mod.getModSize());
			
			if(isLastInstrInMod && !mod.getAllWarningMessages().isEmpty()){
				for(String msg : mod.getAllWarningMessages()){
					System.out.println(msg);
				}
			}
			
		}

	}
	
	private void printUnusedSymbolWarnings(){
		System.out.println();
		for(Symbol symbol : symbolTable.values()){
			if (!symbol.isUsed()){
				System.out.printf("Warning: Module %d: %s was defined but never used\n", 
								  symbol.getModNum(), symbol.getSymbol());
			}
		}
	}
}
