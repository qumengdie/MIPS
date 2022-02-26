#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7

// Memory size.
// In reality, the memory size should be 2^32, but for this lab and space reasons,
// we keep it as this large number, but the memory is still 32-bit addressable.
#define MemSize 65536


class RF
{
  public:
    bitset<32> ReadData1, ReadData2; 
    RF()
    { 
      Registers.resize(32);  
      Registers[0] = bitset<32> (0);  
    }

    void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
    {   
      // TODO: implement!
            ReadData1 = Registers[RdReg1.to_ulong()];
            ReadData2 = Registers[RdReg2.to_ulong()];
        
        if (WrtEnable[0] == 1){
            Registers[WrtReg.to_ulong()] = WrtData;
        }
    }

    void OutputRF()
    {
      ofstream rfout;
      rfout.open("RFresult.txt",std::ios_base::app);
      if (rfout.is_open())
      {
        rfout<<"A state of RF:"<<endl;
        for (int j = 0; j<32; j++)
        {        
          rfout << Registers[j] << endl;
        }

      }
      else cout<<"Unable to open file";
      rfout.close();

    }     
  private:
    vector<bitset<32> >Registers;
};


class ALU
{
  public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {   
      // TODO: implement!
        switch (ALUOP.to_ulong()){
            case ADDU:
                ALUresult = oprand1.to_ulong() + oprand2.to_ulong();
                break;
                
            case SUBU:
                ALUresult = oprand1.to_ulong() - oprand2.to_ulong();
                break;
                
            case AND:
                ALUresult = oprand1 & oprand2;
                break;
                
            case OR:
                ALUresult = oprand1 | oprand2;
                break;
                
            case NOR:
                ALUresult = ~(oprand1 | oprand2);
                break;
        }
        
      return ALUresult;
    }            
};


class INSMem
{
  public:
    bitset<32> Instruction;
    INSMem()
    {      
      IMem.resize(MemSize); 
      ifstream imem;
      string line;
      int i=0;
      imem.open("imem.txt");
      if (imem.is_open())
      {
        while (getline(imem,line))
        {
            if((int)line.length() >= 8)
            {
                if((int)line.length() > 8)
                    line.pop_back();
                IMem[i] = bitset<8>(line);
              ++i;
            }
        }

      }
      else cout<<"Unable to open file";
      imem.close();

    }

    bitset<32> ReadMemory (bitset<32> ReadAddress) 
    {    
      // TODO: implement!
      // (Read the byte at the ReadAddress and the following three byte).
      //
        unsigned long RA = ReadAddress.to_ulong();
        
        Instruction = (IMem[RA].to_ulong() << 24) | (IMem[RA + 1].to_ulong() << 16) | (IMem[RA + 2].to_ulong() << 8) | (IMem[RA + 3].to_ulong());
        
      return Instruction;     
    }     

  private:
    vector<bitset<8> > IMem;

};

class DataMem    
{
  public:
    bitset<32> readdata;  
    DataMem()
    {
      DMem.resize(MemSize); 
      ifstream dmem;
      string line;
      int i=0;
      dmem.open("dmem.txt");
      if (dmem.is_open())
      {
        while (getline(dmem,line))
        {
            if((int)line.length() >= 8)
            {
                if((int)line.length() > 8)
                    line.pop_back();
              DMem[i] = bitset<8>(line);
              ++i;
            }
        }
      }
      else cout<<"Unable to open file";
      dmem.close();

    }  
    bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
    {    
      // TODO: implement!
        unsigned long daddr = Address.to_ulong();
        if (readmem==1){
            readdata = (DMem[daddr].to_ulong() << 24) | (DMem[daddr + 1].to_ulong() << 16) | (DMem[daddr + 2].to_ulong() << 8) | (DMem[daddr + 3].to_ulong());
        }
        else if (writemem==1){
            unsigned long WD = WriteData.to_ulong();
            DMem[daddr] = WD >> 24;
            DMem[daddr + 1] = WD >> 16;
            DMem[daddr + 2] = WD >> 8;
            DMem[daddr + 3] = WD;
        }
        
      return readdata;     
    }   

    void OutputDataMem()
    {
      ofstream dmemout;
      dmemout.open("dmemresult.txt");
      if (dmemout.is_open())
      {
        for (int j = 0; j< 1000; j++)
        {     
          dmemout << DMem[j]<<endl;
        }

      }
      else cout<<"Unable to open file";
      dmemout.close();

    }             

  private:
    vector<bitset<8> > DMem;

};  



int main()
{
  RF myRF;
  ALU myALU;
  INSMem myInsMem;
  DataMem myDataMem;
    
    bitset<32> pc;

  while (1)
  {
    // Fetch
      bitset<32> instruction = myInsMem.ReadMemory(pc);
      pc = pc.to_ulong() + 4;

    // If current instruction is "11111111111111111111111111111111", then break;
      if (instruction.count() == 32)
          break;

    // decode(Read RF)
      bitset<6> OpCode = instruction.to_ulong() >> 26;
      bool IsLoad = OpCode == 35;
      bool IsStore = OpCode == 43;
      bool IsRType = OpCode == 0;
      bool IsJType = OpCode == 2 || OpCode == 3;
      bool IsIType = !IsRType && !IsJType;
      
      bitset<5> Rs = (instruction.to_ulong() >> 21) & 0x1F;
      bitset<5> Rt = (instruction.to_ulong() >> 16) & 0x1F;
      bitset<5> Rd = (instruction.to_ulong() >> 11) & 0x1F;
      bitset<5> Shamt = (instruction.to_ulong() >> 6) & 0x1F;
      bitset<6> RFunc = instruction.to_ulong() & 0x3F;
      bitset<16> Imm = instruction.to_ulong() & 0xFFFF;
      bitset<32> SEImm;
      if (Imm[15]==0)
          SEImm = Imm.to_ulong();
      else
          SEImm = 0xFFFF0000 | Imm.to_ulong();
      bitset<26> JAddr = instruction.to_ulong() & 0x3FFFFFF;

      bitset<3> ALUOP;
      if (IsLoad || IsStore)
          ALUOP = 0b001;
      else if (IsRType)
          ALUOP = RFunc.to_ulong() & 0b111;
      else
          ALUOP = OpCode.to_ulong() & 0b111;
      
      bitset<1> WrtEnable;
      bool IsBranch = OpCode == 0x04;
      if (IsStore || IsBranch || IsJType)
          WrtEnable = 0;
      else
          WrtEnable = 1;
      
      myRF.ReadWrite(Rs, Rt, IsIType ? Rt : Rd, 0, 0);
      bool IsEQ = myRF.ReadData1 == myRF.ReadData2;
      bitset<32> BrAddr = SEImm.to_ulong() << 2;
      if (IsBranch && IsEQ){
          pc = pc.to_ulong() + BrAddr.to_ulong();
          myRF.OutputRF();
          continue;
      }
      else if (IsJType){
          bitset<4> temp = pc.to_ulong() >> 28;
          pc = (temp.to_ulong() << 28 | JAddr.to_ulong() << 2);
          myRF.OutputRF();
          continue;
      }

    // Execute
      bitset<32> ALUresult = myALU.ALUOperation (ALUOP, myRF.ReadData1, IsIType ? SEImm : myRF.ReadData2);

    // Read/Write Mem
      bitset<32> MemReadData = myDataMem.MemoryAccess (ALUresult, myRF.ReadData2, IsLoad, IsStore);
      
    // Write back to RF
      myRF.ReadWrite(Rs, Rt, IsIType ? Rt : Rd, IsLoad ? MemReadData : ALUresult, WrtEnable);
      
      myRF.OutputRF(); // dump RF;
  }
  myDataMem.OutputDataMem(); // dump data mem

  return 0;
}
