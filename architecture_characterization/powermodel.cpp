#include "powermodel.hpp"
PowerModel::PowerModel(interval& it,Config& c) {
    powerModel=it.model;
    sbox=it.sbox;
    step=c.step;    
    keySize=it.keySize;
    keySpace=pow(2,keySize);
    position=it.position;
    intSize=it.intSize;
    key=it.key;
    m=c.m;
    /*printf("0x%x\n",FT0[0]);
    printf("0x%x\n",FT0[5]);*/
}

uint32_t PowerModel::substitute(uint8_t v) {
    if(m==aes)
        return SBOX[v];
    else if(m==ttable)
        return FT0[v];
}


void PowerModel::generate(uint8_t** plaintext,unsigned**powerMatrix) {
    uint32_t ptx,ptx2;
    Operation opCode=none;
    //p is an helper variable that behaves as an offset for
    //the second plaintext (if present)
    int p,s;
    unsigned k;
    /* AES side channel attack mode:
     * position indicates what intermediate values
     * are taken for the power model generation.
     * subX_Y means take the byte X and Y, using
     * the values after subByte stage
     */
    if(m==aes || m==ttable) {
        if(position.compare("ptx")==0)
            p=0;
        else if(position.compare("sub")==0)
            p=2;
        else if(position.compare("sub0_5")==0) {
            sbox=0;
            p=5;
        }
        else if(position.compare("sub0_1")==0) {
            sbox=0;
            p=1;
        }
        else if(position.compare("sub1_0")==0) {
            sbox=1;
            p=0;
        }
        else if(position.compare("sub1_2")==0) {
            sbox=1;
            p=2;
        }
        else if(position.compare("sub1_11")==0) {
            sbox=1;
            p=11;
        }
        else if(position.compare("sub2_1")==0) {
            sbox=2;
            p=1;
        }
        else if(position.compare("sub2_3")==0) {
            sbox=2;
            p=3;
        }
        else if(position.compare("sub3_4")==0) {
            sbox=3;
            p=4;
        }
        else if(position.compare("sub4_5")==0) {
            sbox=4;
            p=5;
        }
        else if(position.compare("sub5_6")==0) {
            sbox=5;
            p=6;
        }
        else if(position.compare("sub6_7")==0) {
            sbox=6;
            p=7;
        }
        else if(position.compare("sub7_8")==0) {
            sbox=7;
            p=8;
        }
        else if(position.compare("sub8_9")==0) {
            sbox=8;
            p=9;
        }
        else if(position.compare("sub9_10")==0) {
            sbox=9;
            p=10;
        }
        else if(position.compare("sub10_11")==0) {
            sbox=10;
            p=11;
        }
        else if(position.compare("sub11_12")==0) {
            sbox=11;
            p=12;
        }
        else if(position.compare("sub12_13")==0) {
            sbox=12;
            p=13;
        }
        else if(position.compare("sub13_14")==0) {
            sbox=13;
            p=14;
        }
        else if(position.compare("sub14_15")==0) {
            sbox=14;
            p=15;
        }
        else if(position.compare("sub15_14")==0) {
            sbox=15;
            p=14;
        }
        else if(position.compare("sub5_9")==0) {
            sbox=5;
            p=9;
        }
        else if(position.compare("sub9_13")==0) {
            sbox=9;
            p=13;
        }
        else if(position.compare("sub13_1")==0) {
            sbox=13;
            p=1;
        }
        else if(position.compare("sub10_14")==0) {
            sbox=10;
            p=14;
        }
        else if(position.compare("sub10_3")==0) {
            sbox=10;
            p=3;
        }
        else if(position.compare("sub14_2")==0) {
            sbox=14;
            p=2;
        }
        else if(position.compare("sub14_7")==0) {
            sbox=14;
            p=7;
        }
        else if(position.compare("sub2_6")==0) {
            sbox=2;
            p=6;
        }
        else if(position.compare("sub15_3")==0) {
            sbox=15;
            p=3;
        }
        else if(position.compare("sub3_7")==0) {
            sbox=3;
            p=7;
        }
        else if(position.compare("sub7_11")==0) {
            sbox=7;
            p=11;
        }
        else if(position.compare("sub5_10")==0) {
            sbox=5;
            p=10;
        }
        else if(position.compare("sub15_0")==0) {
            sbox=15;
            p=0;
        }
        else if(position.compare("sub10_15")==0) {
            sbox=10;
            p=15;
        }
        else if(position.compare("sub4_9")==0) {
            sbox=4;
            p=9;
        }
        else if(position.compare("sub9_14")==0) {
            sbox=9;
            p=14;
        }
        else if(position.compare("sub14_3")==0) {
            sbox=14;
            p=3;
        }
        else if(position.compare("sub8_13")==0) {
            sbox=8;
            p=13;
        }
        else if(position.compare("sub13_2")==0) {
            sbox=13;
            p=2;
        }
        else if(position.compare("sub2_7")==0) {
            sbox=2;
            p=7;
        }
        else if(position.compare("sub12_1")==0) {
            sbox=12;
            p=1;
        }
        else if(position.compare("sub12_6")==0) {
            sbox=12;
            p=6;
        }
        else if(position.compare("sub11_12")==0) {
            sbox=11;
            p=12;
        }
        else if(position.compare("sub1_6")==0) {
            sbox=1;
            p=6;
        }
        else if(position.compare("sub6_11")==0) { 
            sbox=6;
            p=11;
        }
        else {
            cout<<"Position "<<position<<" not recognized."<<endl
            <<"Maybe you're using a known input position for a key guess attack?"<<endl;
            exit(0);
        }
    }
    /* characterization mode: the considered value
     * is the plain, during characterization the AES
     * algorithm is generally not used 
     */
    else if(m==charac) {
        //in case of ptxX_Y, sbox param is not necessary, so reset it
        if(position.compare("ptx1_2")==0) {
            sbox=0;
            p=4;
        }
        else if(position.compare("ptx1_3")==0) {
            sbox=0;
            p=8;
        }
        else if(position.compare("ptx1_4")==0) {
            sbox=0;
            p=12;
        }
        else if(position.compare("ptx2_3")==0) {
            sbox=4;
            p=8;
        }
        else if(position.compare("ptx2_4")==0) {
            sbox=4;
            p=12;
        }
        else if(position.compare("ptx3_4")==0) {
            sbox=8;
            p=12;
        }
        else if(position.compare("ptx4_1")==0) {
            sbox=12;
            p=0;
        }
        else if(position.compare("ptx4_2")==0) {
            sbox=12;
            p=4;
        } else if(position.compare("addptx1_2")==0) {
            sbox=0;
            p=4;
            opCode=add;
        } else if(position.compare("xorptx1_2")==0) {
            sbox=0;
            p=4;
            opCode=bitwise_xor;
        } else if(position.compare("andptx1_2")==0) {
            sbox=0;
            p=4;
            opCode=bitwise_and;
        } else if(position.compare("mulptx1_2")==0) {
            sbox=0;
            p=4;
            opCode=mul;
        } else if(position.compare("shiftptx1_2")==0) {
            sbox=0;
            p=4;
            opCode=shift;
        } else if(position.compare("shiftptx2_3")==0) {
            sbox=4;
            p=8;
            opCode=shift;
        } else if(position.compare("addptx1_3")==0) {
            sbox=0;
            p=8;
            opCode=add;
        } else if(position.compare("addptx1_4")==0) {
            sbox=0;
            p=12;
            opCode=add;
        } else if(position.compare("addptx2_3")==0) {
            sbox=4;
            p=8;
            opCode=add;
        } else if(position.compare("addptx2_4")==0) {
            sbox=4;
            p=12;
            opCode=add;
        } else if(position.compare("addptx3_4")==0) {
            sbox=8;
            p=12;
            opCode=add;
        } else if(position.compare("xorload1_2")==0) {
            sbox=0;
            p=4;
            opCode=value_and_xor;
        } else if(position.compare("addall")==0) {
            uint32_t ptx3,ptx4;
            for(s=0;s<step;s++) {
            //for each key hypothesis
                for(k=0;k<keySpace;k++) {
                    ptx=ptx2=0;
                    //in case of an attack, the second input is not used
                    computeUsedPlaintext(ptx,ptx2,plaintext[s]+0,plaintext[s]+4,add);
                    computeUsedPlaintext(ptx3,ptx4,plaintext[s]+8,plaintext[s]+12,add);
                    //if the intermediate size is 8bit, use the single bytes
                    //model with known key
                    powerMatrix[s][k]=hammingDistance(ptx,ptx3);               
                }
            }
            return;
        }
        //TODO:insert here sbox characterization
            
        else {
            cout<<"Position "<<position<<" not recognized."<<endl
            <<"Maybe you're using an AES position for a known input attack?."<<endl;
            exit(0);
        }
        /*if(opCode>0 && powerModel.compare("hd")==0) {
            cout<<"Requested hamming distance of the result of an operation."<<endl;
            exit(0);
        }*/
    }
    uint8_t k1,k2;
    //if the model is the hamming weight
    if(powerModel.compare("hw")==0) {
        //for each plaintext
        for(s=0;s<step;s++) {
            //for each key hypothesis
            for(k=0;k<keySpace;k++) {
                ptx=ptx2=0;
                //in case of an attack, the second input is not used
                computeUsedPlaintext(ptx,ptx2,plaintext[s]+sbox,plaintext[s]+p,opCode);
//                 computeKey(k,k1,k2);
                //if the intermediate size is 8bit, use the single bytes
                //model with known key
                if(m==charac) {
                    powerMatrix[s][k]=hammingWeight(ptx);
                }
                //otherwise use AES primitives
                else if(p==0) {
                    powerMatrix[s][k]=hammingWeight(ptx^k);
                }
                else {
                    //take the plaintext, xor with the key and substitution.
                    powerMatrix[s][k]=hammingWeight(substitute(ptx^k));
                }
            }
        }
    }
    //if the model is the hamming distance
    else if(powerModel.compare("hd")==0) {
        //for each plaintext
        for(s=0;s<step;s++) {
            //for each key hypothesis
            for(k=0;k<keySpace;k++) {
                ptx=ptx2=0;
                //in case of an attack, the second plaintext is not used
                computeUsedPlaintext(ptx,ptx2,plaintext[s]+sbox,plaintext[s]+p,opCode);
//                 computeKey(k,k1,k2);
                if(m==charac)   //p==0 only for completness,no sense here
                    powerMatrix[s][k]=hammingDistance(ptx,ptx2);
                else if(p==0)
                    powerMatrix[s][k]=hammingDistance(ptx,k);
                else {
                    //attack subbytes, 8 bit attack, one part of the key is fixed (it's a cheat)
                    if(sbox==0 && p==1)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x2b),
                                                      substitute(ptx2^k));
                    else if(sbox==1 && p==0)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x7e),
                                                      substitute(ptx2^k));
                    else if(sbox==1 && p==2)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x7e),
                                                     substitute(ptx2^k));
                    else if(sbox==2 && p==3)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x15),
                                                     substitute(ptx2^k));
                    else if(sbox==3 && p==4)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x16),
                                                     substitute(ptx2^k));
                    else if(sbox==4 && p==5)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x28),
                                                     substitute(ptx2^k));
                    else if(sbox==5 && p==6)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xae),
                                                     substitute(ptx2^k));
                    else if(sbox==6 && p==7)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xd2),
                                                     substitute(ptx2^k));
                    else if(sbox==7 && p==8)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xa6),
                                                     substitute(ptx2^k));
                    else if(sbox==8 && p==9)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xab),
                                                     substitute(ptx2^k));
                    else if(sbox==9 && p==10)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xf7),
                                                     substitute(ptx2^k));
                    else if(sbox==10 && p==11)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x15),
                                                     substitute(ptx2^k));
                    else if(sbox==11 && p==12)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x88),
                                                     substitute(ptx2^k));
                    else if(sbox==12 && p==13)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x09),
                                                     substitute(ptx2^k));
                    else if(sbox==13 && p==14)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xcf),
                                                     substitute(ptx2^k));
                    else if(sbox==14 && p==15)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x4f),
                                                     substitute(ptx2^k));
                    else if(sbox==15 && p==14)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x3c),
                                                     substitute(ptx2^k));
                    //attack shift rows
                    else if(sbox==5 && p==9)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xae),
                                                     substitute(ptx2^k));
                    else if(sbox==9 && p==13)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xf7),
                                                     substitute(ptx2^k));
                    else if(sbox==13 && p==1)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xcf),
                                                     substitute(ptx2^k));
                    else if(sbox==10 && p==14)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x15),
                                                     substitute(ptx2^k));
                    else if(sbox==14 && p==2)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x4f),
                                                     substitute(ptx2^k));
                    else if(sbox==2 && p==6)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x15),
                                                     substitute(ptx2^k));
                    else if(sbox==15 && p==3)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x3c),
                                                     substitute(ptx2^k));
                    else if(sbox==3 && p==7)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x16),
                                                     substitute(ptx2^k));
                    else if(sbox==7 && p==11)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xa6),
                                                     substitute(ptx2^k));
                    else if(sbox==0 && p==5)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x2b),
                                                     substitute(ptx2^k));
                    else if(sbox==5 && p==10)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xae),
                                                     substitute(ptx2^k));
                    else if(sbox==10 && p==15)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x15),
                                                     substitute(ptx2^k));
                    else if(sbox==4 && p==9)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x28),
                                                     substitute(ptx2^k));
                    else if(sbox==9 && p==14)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xf7),
                                                     substitute(ptx2^k));
                    else if(sbox==14 && p==3)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x4f),
                                                     substitute(ptx2^k));
                    else if(sbox==8 && p==13)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xab),
                                                     substitute(ptx2^k));
                    else if(sbox==13 && p==2)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xcf),
                                                     substitute(ptx2^k));
                    else if(sbox==2 && p==7)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x15),
                                                     substitute(ptx2^k));
                    else if(sbox==12 && p==1)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x09),
                                                     substitute(ptx2^k));
                    else if(sbox==1 && p==6)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x7e),
                                                     substitute(ptx2^k));
                    else if(sbox==6 && p==11)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0xd2),
                                                     substitute(ptx2^k));
                    else if(sbox==15 && p==0)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x3c),
                                                     substitute(ptx2^k));
                    else if(sbox==10 && p==3)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x15),
                                                     substitute(ptx2^k));
                    else if(sbox==14 && p==7)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x4f),
                                                     substitute(ptx2^k));
                    else if(sbox==2 && p==1)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x15),
                                                     substitute(ptx2^k));
                    else if(sbox==1 && p==11)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x7e),
                                                     substitute(ptx2^k));
                    else if(sbox==11 && p==12)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x88),
                                                     substitute(ptx2^k));
                    else if(sbox==12 && p==6)
                        powerMatrix[s][k]=hammingDistance(substitute(ptx^0x09),
                                                     substitute(ptx2^k));    
                }
            }
        }        
    }
    //the model is the sum of different power consumptions
    else if(powerModel.compare("hd_sum")==0) {
        uint32_t ptx3,ptx4,ptx5,ptx6;
        for(s=0;s<step;s++) {
            //for each key hypothesis
            for(k=0;k<keySpace;k++) {
                ptx=ptx2=ptx3=ptx4=ptx5=ptx6=0;
                //if ptx1_3
                if(p==8) {
                    computeUsedPlaintext(ptx,ptx2,plaintext[s],plaintext[s]+4,none);
                    computeUsedPlaintext(ptx3,ptx4,plaintext[s]+4,plaintext[s]+8,none);
                } 
                //else if ptx1_4
                else if(p==12) {
                    computeUsedPlaintext(ptx,ptx2,plaintext[s],plaintext[s]+4,none);
                    computeUsedPlaintext(ptx3,ptx4,plaintext[s]+4,plaintext[s]+8,none);
                    computeUsedPlaintext(ptx5,ptx6,plaintext[s]+8,plaintext[s]+12,none);
                }
                else {
                    cout<<"Using an hamming distance sum model: as a position use ptx1_3"
                    <<"to consider the three 32-bit plaintext,"
                    <<"or ptx1_4 to consider all the 4 words"<<endl;
                    exit(0);
                }
                powerMatrix[s][k]=hammingDistance(ptx,ptx2)
                                        +hammingDistance(ptx3,ptx4);
                                        +hammingDistance(ptx5,ptx6);
            }
        }           
    }
    else {
        cout<<"Interval model not recognized."<<endl;
        exit(0);
    }
}

void PowerModel::computeKey(uint16_t k,uint8_t& k1,uint8_t& k2) {
    k2=k&0x00ff;
    k>>=8;
    k1=k&0x00ff;
}
//the opCode is passed in case the user requires the hw of some results
void PowerModel::computeUsedPlaintext(uint32_t& intermediate,uint32_t& intermediate2,
                                      uint8_t*plaintext,uint8_t*plaintext2,Operation opCode) {
    //fill the integer buffer with the correct numbers
    for(int w=0;w<intSize/8;w++) {
        intermediate<<=8;
        intermediate2<<=8;
        intermediate|=plaintext[w];
        intermediate2|=plaintext2[w];
    }
    if(opCode!=0) {
        switch(opCode) {
            case add:
                intermediate+=intermediate2;
                break;
            case bitwise_xor:
                intermediate^=intermediate2;
                break;
            case bitwise_and:
                intermediate&=intermediate2;
                break;
            case mul:
                intermediate*=intermediate2;
                break;
            case value_and_xor:
                intermediate2=intermediate^intermediate2;
                break;
            case shift:
                intermediate<<=3;
                intermediate2<<=3;
                break;
            default:
                cout<<"Operation not recognized."<<endl;
                exit(0);
        }
    }
}

unsigned PowerModel::hammingWeight(uint32_t intermediate) {
    unsigned int weight = 0;
    while (intermediate) {
        intermediate = intermediate & (intermediate-1);
        weight++;
    }
    return weight;
}

unsigned PowerModel::hammingDistance(uint32_t x,uint32_t y) {
    return this->hammingWeight(x^y);
}