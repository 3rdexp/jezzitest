#!/usr/bin/python
#coding:utf-8

def BuildTree(arr):
  """ 很特殊的转换树的函数, TODO: 'p' 参数化
  >>> t=BuildTree([{}, {'p':0}, {'p':0}, {'p':1}, {}, {'p':0}])
  >>> DumpTree(t)
  0
   1
    3
   2
   5
  4
  >>> t=BuildTree([{}])
  >>> DumpTree(t)
  0
  >>> t=BuildTree([{}, {}])
  >>> DumpTree(t)
  0
  1
  >>> t=BuildTree([{}, {'p':0}])
  >>> DumpTree(t)
  0
   1
  >>> t=BuildTree([{}, {}, {'p':1}])
  >>> DumpTree(t)
  0
  1
   2
  >>> t=BuildTree([{}, {'p':2}, {}]) # 诡异的情形，早期的条目居然是后来的子节点
  >>> DumpTree(t)
  0
  2
  """
  
  tree = []
  for i in range(0, len(arr)):
    a = arr[i]
    a['i'] = i
    if 'p' not in a:
      tree.append(a)
    else:
       # 处理诡异情形
      if len(tree) < a['p'] or not isinstance(a['p'], int):
        continue
  
      p = arr[a['p']]
      if 'child' not in p:
        p['child'] = []

      p['child'].append(a)
  return tree


def DumpTree(t, indent=0, f=lambda item: item['i']):
  for i in t:
    print "%s%s" % (' ' * indent, f(i))

    if 'child' in i:
      DumpTree(i['child'], 1+indent)


if __name__ == "__main__":
  # t=BuildTree([{}, {'p':0}, {'p':0}, {'p':1}, {}, {'p':0}])
  # DumpTree(t)
  import doctest
  doctest.testmod()
