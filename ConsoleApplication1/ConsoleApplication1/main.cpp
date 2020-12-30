#include<SFML/Graphics.hpp>//处理图像的头文件
#include<SFML/Audio.hpp>//处理声音文件
#include<time.h>//处理时间文件
using namespace sf;
//旋转，消除，加分，音乐
const int ROW_COUNT = 20;
const int COL_COUNT = 10;
//下降速度
const float SPEED_NORMAL = 0.3;
const float SPEED_QUICK = 0.05;
float delay = SPEED_NORMAL;

int blockIndex;//当前方块的种类
int blocks[7][4] = {//定义七种方块的数组
	{1,3,5,7},//I
	{2,4,5,7},//Z
	{3,5,4,6},//Z
	{3,5,4,7},//T
	{2,3,5,7},//L
	{3,5,7,6},//J
	{2,3,4,5}//田
};
int table[ROW_COUNT][COL_COUNT] = { 0 };//游戏区域的表示
				  //若table[i][j]=0则下标为ij的空白，并用值表示颜色取值
struct Point {//俄罗斯方块的表示（都是4小块的）位置不断发生变化

	int x;
	int y;
} curBlock[4],bakBlock[4];//设置一个备份block

Sound sou;//消除声音显示
//得分显示
Font font;
Text textScore;
int score = 0;

bool check() {//检查是不是编写
	for (int i = 0; i < 4; i++) {
		if (curBlock[i].x < 0 || curBlock[i].x >= COL_COUNT || curBlock[i].y >= ROW_COUNT || curBlock[i].y <= 0 || table[curBlock[i].y][curBlock[i].x])	//超出边界//会不会重复位置
			//超出边界//会不会重复位置
		{
			return false;
		}
	}
	return true;
}

void moveLeftRight(int dx) {
	for (int i = 0; i < 4; i++) {
		bakBlock[i] = curBlock[i];//备份
		curBlock[i].x += dx;
	}
	if (!check()) {
		for (int i = 0; i < 4; i++) {
			curBlock[i] = bakBlock[i];//备份
		}
	}
}
//1.空间换时间，把所有形态存下来
//速度快，占用内存大
//2.算法进行旋转
void doRotate() {
	if (blockIndex == 7) {  // 田字形，不需要旋转
		return;
	}
	//备份当前方块，出问题再回退
	for (int i = 0; i < 4; i++) {
		bakBlock[i] = curBlock[i];  
	}

	Point p = curBlock[1]; //旋转中心
	//a[i].x=p.x-a[i].y+p.x
	//a[i].y=p.y+a[i].x-p.x;
	for (int i = 0; i < 4; i++)
	{
		struct Point tmp = curBlock[i];
		curBlock[i].x = p.x - tmp.y + p.y;
		curBlock[i].y = p.y + tmp.x - p.x;
	}
	if (!check()) {
		for (int i = 0; i < 4; i++) {
			curBlock[i] = bakBlock[i];
		}
	}
}
//序号转换
void keyEvent(RenderWindow *window) {//按键反应
	bool rotate = false;//是否旋转
	int dx = 0;//偏移量
	Event e;//事件变量
	//pollEvent 从队列里面返回一个事件
	//无事件 false
	while (window->pollEvent(e)) {
		if (e.type == Event::Closed)//事件是关闭窗口
		{
			window->close();
		}
		if (e.type == Event::KeyPressed)
		{
			switch (e.key.code) {
			case Keyboard::Up:
				rotate = true;
				break;
			case Keyboard::Left:
				dx = -1;
				break;
			case Keyboard::Right:
				dx = 1;
				break;
			default:
				break;
			}
		}
		//一直下降
		if (Keyboard::isKeyPressed(Keyboard::Down)) {
			delay = SPEED_QUICK;//快速
		}

		if (dx != 0) {
			moveLeftRight(dx);
		}

		//旋转操作
		if (rotate) {
			doRotate();
		}
	}
}

void newBlock() {//生成方块

	//随机生成
	blockIndex = 1 + rand() % 7;
	int n = blockIndex - 1;
	for (int i = 0; i < 4; i++) {
		curBlock[i].x = blocks[n][i] % 2;
		curBlock[i].y = blocks[n][i] / 2;
	}
}

void drawBlocks(RenderWindow* window, Sprite* spriteBlock) {
	// 绘制已降落完毕的方块
	for (int i = 0; i < ROW_COUNT; i++)
		for (int j = 0; j < COL_COUNT; j++)
		{
			if (table[i][j] == 0) continue;
			spriteBlock->setTextureRect(IntRect(table[i][j] * 18, 0, 18, 18));
			spriteBlock->setPosition(j * 18, i * 18);
			spriteBlock->move(28, 31); //调整边框位置
			window->draw(*spriteBlock);
		}
	// 绘制当前方块
	for (int i = 0; i < 4; i++)
	{
		spriteBlock->setTextureRect(IntRect(blockIndex * 18, 0, 18, 18));
		spriteBlock->setPosition(curBlock[i].x * 18, curBlock[i].y * 18);//用其切割一个小方块
		spriteBlock->move(28, 31); //调整边框位置
		window->draw(*spriteBlock);
	}
}
void drop() {
	//y坐标加一就可以
	for (int i = 0; i < 4; i++) {
		bakBlock[i] = curBlock[i];
		curBlock[i].y += 1;
	}
	//不能穿过地上
	if (check() == false) {
		//固定处理
		for (int i = 0; i < 4; i++) {
			table[bakBlock[i].y][bakBlock[i].x] = blockIndex;
		}
		//产生新方块
		newBlock();
	}
}
void clearLine() {
	int k = ROW_COUNT - 1;//重新写方块
	for (int i = ROW_COUNT - 1; i > 0; i--) {
		int count = 0;
		for (int j = 0; j < COL_COUNT; j++) {
			if (table[i][j]) {
				count++;
			}
			table[k][j] = table[i][j];//一遍统计一边写一编
		}
		if (count < COL_COUNT) {
			k--;//拿去覆盖
		}
		else {
			score += 10;
			sou.play();//放消除音效
		}
	}
	//显示分数
	char tmp[16];
	sprintf_s(tmp, "%d", score);
	textScore.setString(tmp);
}
void initScore() {
	if (!font.loadFromFile("C:/Users/Hey/Desktop/俄罗斯方块/ConsoleApplication1/Sansation.ttf")) {
		exit(1);
	}

	textScore.setFont(font); // font is a sf::Font
	textScore.setCharacterSize(30);// set the character size
	textScore.setFillColor(sf::Color::Black); // set the color
	textScore.setStyle(sf::Text::Bold); // set the text style
	textScore.setPosition(255, 175);
	textScore.setString("0");
}

int main(void) {

	//游戏背景音
	Music music;
	if (!music.openFromFile("C:/Users/Hey/Desktop/俄罗斯方块/ConsoleApplication1/bg2.wav"))
	{
		return -1;
	}
	music.setLoop(true);//循环播放
	music.play();

	SoundBuffer xiaochu;
	if(!xiaochu.loadFromFile("C:/Users/Hey/Desktop/俄罗斯方块/ConsoleApplication1/bg.wav")){
		return -1;
	}

	srand(time(0));//用当前时间生成随机种子
	RenderWindow window(VideoMode(320, 416),"HEY");//创建窗口

	Texture bg;//加载背景图片
	bg.loadFromFile("C:/Users/Hey/Desktop/俄罗斯方块/ConsoleApplication1/image/bg.jpg");
	Sprite spriteBg(bg);//根据图片创造对象

	Texture tile;//加小方块图片
	tile.loadFromFile("C:/Users/Hey/Desktop/俄罗斯方块/ConsoleApplication1/image/tiles.png");
	Sprite spriteBlock(tile);//根据图片创造对象
	
	Texture bk;//加小方块图片
	bk.loadFromFile("C:/Users/Hey/Desktop/俄罗斯方块/ConsoleApplication1/image/bk.png");
	Sprite spriteFrame(bk);//根据图片创造对象
	//window.draw(spriteBg);
	//window.display();//
	initScore();

	//生成方块
	newBlock();
	//设置计时器
	Clock clock;
	float timer = 0;

	//进入游戏循环
	//进入游戏
	while (window.isOpen()) {//如果窗口没有被关闭

		//启动到现在的时间
		float time = clock.getElapsedTime().asSeconds();
		clock.restart(); 
		timer += time;
		
		//等待用户按键
		keyEvent(&window);//左右移动和旋转

		if (timer > delay)
		{
			//降落
			drop();//下降一个位置
			timer = 0;
		}
		for (int i = 0; i < 10; i++) {
			if (table[0][i]||table[1][0]|| table[1][1] || table[2][0]||table[2][1]||table[3][0]||table[3][1] ) {
				printf("游戏结束，最终得分为%d\n", score);
				system("pause");
				return -1;
			}
		}
		//消分处理
		clearLine();
		delay = SPEED_NORMAL;//速度还原
		window.draw(spriteBg);
		window.draw(spriteFrame);
		//绘制方块，绘制游戏
		drawBlocks(&window,&spriteBlock);
		//渲染方块
		window.draw(textScore);//显示分数
		window.display();//
	}
	system("pause");
	return 0;
}