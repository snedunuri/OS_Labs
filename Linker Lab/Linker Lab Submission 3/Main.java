package main;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.util.Scanner;

import org.apache.commons.lang3.StringUtils;

public class Main {

	private static ByteArrayOutputStream outContent = new ByteArrayOutputStream();
	private static ByteArrayOutputStream errContent = new ByteArrayOutputStream();
	private static boolean isTest = false;
	
	public static void main(String[] args) throws IOException{
		
		if(isTest){
			for(int i = 0; i < 20; i++){
				String[] arr = new String[1];
				arr[0] = String.format("/Users/steja_nedunuri/Documents/Graduate School/Semester III/"
						+ "Operating Systems/Linker Lab/labsamples/input-%d", i+1);
				
				System.setOut(new PrintStream(outContent));
				System.setErr(new PrintStream(errContent));
				
				try{
					driver(arr);
				} catch(Exception e){
					
				}
		
				String myOutput = outContent.toString();
				
				PrintWriter pw = new PrintWriter(String.format("/Users/steja_nedunuri/Documents/"
						+ "Graduate School/Semester III/Operating Systems/Linker Lab/my_outputs/out-%d", i+1));
				
				pw.write(myOutput);
				pw.flush();
				pw.close();
				
				//load in actual output
				File targetOutputFile = new File(String.format("/Users/steja_nedunuri/Documents/"
						+ "Graduate School/Semester III/Operating Systems/Linker Lab/labsamples/out-%d", i+1));
				
				StringBuilder targetOutputBuilder = new StringBuilder();
				Scanner scanner = new Scanner(targetOutputFile);
				while(scanner.hasNextLine()){
					targetOutputBuilder.append(scanner.nextLine());
					targetOutputBuilder.append("\n");
				}
				scanner.close();
				
				String targetOutput = targetOutputBuilder.toString();
				
				
				String diff = StringUtils.difference(myOutput, targetOutput);
				
				
				if(!diff.equals("\n")){
					PrintWriter pw2 = new PrintWriter(String.format("/Users/steja_nedunuri/Documents/"
							+ "Graduate School/Semester III/Operating Systems/Linker Lab/diffOutputs/diffout-%d", i+1));
					
					StringBuilder finalOutput = new StringBuilder();
					
					finalOutput.append(myOutput);
					finalOutput.append("***********************************");
					finalOutput.append("\n");
					finalOutput.append(targetOutput);
					finalOutput.append("***********************************");
					finalOutput.append("\n");
					finalOutput.append(diff);
					
					pw2.write(finalOutput.toString());
					
					pw2.flush();
					pw2.close();
				}
				
				outContent.reset();
				errContent.reset();
				
			}
		} else {
			driver(args);
		}
		
	}
	
	public static void driver(String[] args) throws IOException{
		
		//check to see the correct number of arguments are given
		if(args.length  != 1 ){
			System.out.println("Error: Incorrect number of arguments given. "
					+ "This program takes exactly one argument");
			System.exit(-1);
		}
		
		//check to see input is a file that exists and can be read
		File input = new File(args[0]);
		if(!input.exists()){
			System.out.println("Error: The input file does not exist");
			System.exit(-1);
		}
		if(!input.isFile()){
			System.out.println("Error: The given argument is not a file");
			System.exit(-1);
		}
		if(!input.canRead()){
			System.out.println("Error: The given file cannot be read");
			System.exit(-1);
		}
		
		Linker parser = new Linker(input);
		
		parser.run();
		
		
	}
}
