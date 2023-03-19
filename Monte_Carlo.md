### Monte Carlo Tree Search

- 蒙特卡洛树搜索是一类树搜索算法的统称，简称MCTS。它是一种用于 ***决策过程*** 的启发式算法，且在搜索空间巨大的游戏中会比较有效
- 从全局来看，蒙特卡洛树搜索的主要目标是：给定一个游戏状态来选择最佳的下一步。
- 常见应用包括Alpha Go，象棋或围棋AI程序等



#### 算法过程

- 选择（Selection）：选择能够最大化UCB值的结点

​		![截屏2023-03-18 22.49.45](/Users/zhangjinkai/Library/Application Support/typora-user-images/截屏2023-03-18 22.49.45.png)

​		V_i：该结点下的平均value大小

​		c：常数，通常可以取2

​		N：总探索次数

​		n_i：当前结点探索次数



- 扩展（Node Expansion）：创建一个或多个子结点
- 仿真（Simulation）：在某一结点用随机策略进行游戏，又称为*play out*或*roll out*
- 反向传播（Backpropagation）：是用随机搜索的结果来更新整个搜索树

![截屏2023-03-18 22.58.03](/Users/zhangjinkai/Library/Application Support/typora-user-images/截屏2023-03-18 22.58.03.png)

```C++
Def Rollout(S_i):
	loop forever:
		if S_i is a terminal state://game over
			return value(S_i)//evaluate function
    else 
      A_i = random(available-actions(S_i))
      //随机选取可能的动作
    	S_i = simulate(A_i, S_i)
      //通过当前的状态S_i以及随机选取的A_i选择下一步动作
```

#### 算法何时终结

- 游戏内棋手的限制时间（例如 1000ms）

- 给定固定的迭代次数
- 迭代完成后，选择value更大的结点即可结束决策