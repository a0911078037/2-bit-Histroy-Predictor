## 2-bit-Histroy-Predictor
2-bit Histroy Predictor to RISV-V

## 如何使用
* 輸入Ins直到EOF
* 輸入entry
* 會輸出所有entries，並會一直輸出當前的Ins
* 當輸出 'Done' 後代表程式結束

## 程式運作方式
* 輸入完Ins後，會開始分割Ins裡的資訊，存入Class `instruction`
* 讀入entry，確認Tag的數量後，將`instruction`裡的每個address進行分割
* BTB reisze 來對應 entry 的數量
* 開始跑Ins，一開始先由tag來看BTB有沒有對應的資訊 <br>
如果有->進行預測<br>
如果沒有->在程式跑完後，確認是branch便存入資訊，並進行預測<br>
最後->結果跟預測答案進行比對，如果和預測結果不同便進行update

## 程式解釋
```C++
class instruction {
public:
	string ins;   //Ins 例如:add,addi
	string rd;    //Ins 抓到的第一個變數
	string rs1;   //Ins 抓到的第二個變數
	string rs2;   //Ins 抓到的第三個變數 如果沒有會預設空字串
	string ins_address; //Ins的tag
}
```
```C++
class BTB {
public:
	string BTB_table[4] = { "SN","SN","SN","SN" };       // BTB
	string Tag = "";
	int misprediction = 0;                            //失誤的數量
	int branch_counter = 0;                           //00,01,10,11
	bool branch_or_not[2] = { false,false };        //history table
	bool predict = false;							//store the predict
};
```
