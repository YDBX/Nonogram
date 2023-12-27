from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver import ActionChains
from z3 import *
import time
import argparse

import requests
from bs4 import BeautifulSoup

def solve(m, n, r, c):
    # m, n = 0, 0
    # r, c = [], []
    # with open(args.input, 'r') as f:
    #     for i, line in enumerate(f.read().splitlines()):
    #         if i == 0:
    #             m, n = [int(j) for j in line.split(' ')]
    #         elif i <= m:
    #             r.append([int(j) for j in line.split(' ')])
    #         else:
    #             c.append([int(j) for j in line.split(' ')])
    # print(m, n)
    # print(r)
    # print(c)

    x = [[Int(f'x_{i}_{j}') for j in range(n)] for i in range(m)]
    l = [[Int(f'l_{i}_{k}') for k in range(len(clue))] for i, clue in enumerate(r)]
    t = [[Int(f't_{j}_{k}') for k in range(len(clue))] for j, clue in enumerate(c)]
    # print(x)
    # print(l)
    # print(t)
    solver = Solver()
    # range of x_{i, j}
    for i in range(m):
        for j in range(n):
            solver.add(x[i][j] >= 0, x[i][j] <= 1)
    
    # range of l_{i, k}
    for i, clue in enumerate(r):
        for k in range(len(clue)):
            solver.add(l[i][k] >= 0, l[i][k] <= n - clue[-1])
    
    # range of t_{j, k}
    for j, clue in enumerate(c):
        for k in range(len(clue)):
            solver.add(t[j][k] >= 0, t[j][k] <= m - clue[-1])
    
    # Blocks cannot overlap
    for i in range(m):
        for k in range(len(r[i]) - 1):
            solver.add(l[i][k] + r[i][k] < l[i][k + 1])
    for j in range(n):
        for k in range(len(c[j]) - 1):
            solver.add(t[j][k] + c[j][k] < t[j][k + 1])
    
    # (i, j) cell is colored iff it is contained in some block
    for i in range(m):
        for j in range(n):
            tmp = [And(l[i][k] <= j, j < l[i][k] + r[i][k]) for k in range(len(r[i]))]
            solver.add(Implies(x[i][j] == 1, Or(tmp)))
            solver.add(Implies(Or(tmp), x[i][j] == 1))
    for i in range(m):
        for j in range(n):
            tmp = [And(t[j][k] <= i, i < t[j][k] + c[j][k]) for k in range(len(c[j]))]
            solver.add(Implies(x[i][j] == 1, Or(tmp)))
            solver.add(Implies(Or(tmp), x[i][j] == 1))
    
    print(solver.check())
    if solver.check() == sat:
        result = solver.model()
    return result, x
    
        # with open(args.output, 'w') as f:
        #     for i in range(m):
        #         for j in range(n):
        #             if result[x[i][j]] == 1:
        #                 f.write("1")
        #                 print("■ ", end='')
        #             else:
        #                 f.write("0")
        #                 print("□ ", end='')
        #         print()
        #         f.write('\n')

def newBrowserAndSolve(args):
    r, c = [], []
    driver = webdriver.Firefox()
    driver.get(args.url)

    left = driver.find_element(By.ID, "taskLeft")
    groups = left.find_elements(By.XPATH, '*')
    for group in groups:
        clues = group.find_elements(By.XPATH, "*")
        tmp = []
        for clue in clues:
            if len(clue.text):
                tmp.append(int(clue.text))
        r.append(tmp)
    print(r)

    top = driver.find_element(By.ID, "taskTop")
    groups = top.find_elements(By.XPATH, '*')
    for group in groups:
        clues = group.find_elements(By.XPATH, "*")
        tmp = []
        for clue in clues:
            if len(clue.text):
                tmp.append(int(clue.text))
        c.append(tmp)
    print(c)

    start_time = time.time()
    result, x = solve(len(r), len(c), r, c)
    end_time = time.time()
    print(f'solve time: {round(end_time - start_time, 2)}s')

    cell = driver.find_element(By.CLASS_NAME, "nonograms-cell-back")
    rows = cell.find_elements(By.XPATH, "*")
    for i, row in enumerate(rows):
        cols = row.find_elements(By.XPATH, "*")
        # for k, block in enumerate(r[i]):
        #     ActionChains(driver).drag_and_drop(cols[result[l[i][k]].as_long()], cols[result[l[i][k]].as_long() + block - 1]).perform()
        for j, col in enumerate(cols):
            if result[x[i][j]] == 1:
                col.click()
    
    # check
    driver.find_element(By.ID, "btnReady").click()


def write_in(args):

    res = requests.get(args.url)
    soup = BeautifulSoup(res.content, "html.parser")
    script = soup.find_all("script")
    s = str(script)
    
    start = s.find('task = ') + 8
    end = s.find("'; var loadedId")
    data = s[start:end]
    data = data.split('/')
    
    width = s.find('puzzleWidth: ') + len("puzzleWidth: ")
    height = s.find(', puzzleHeight: ')
    width = int(s[width:height])
    height = len(data) - width

    c = [data[i].split('.') for i in range(width)]
    r = [data[i].split('.') for i in range(width, width+height)]
    c = [[int(j) for j in c[i]] for i in range(len(c))]
    r = [[int(j) for j in r[i]] for i in range(len(r))]
    print(f"r = {r}")
    print(f"c = {c}")

    start_time = time.time()
    result, x = solve(height, width, r, c)
    end_time = time.time()
    print(f'solve time: {round(end_time - start_time, 2)}s')

    for i in range(height):
        for j in range(width):
            if result[x[i][j]] == 1:
                print("■ ", end='')
            else:
                print("□ ", end='')
        print()

def main(args):

    if args.write:
        write_in(args)
    else:
        newBrowserAndSolve(args)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--url', type=str)
    parser.add_argument('--write', action='store_true')
    args = parser.parse_args()
    main(args)