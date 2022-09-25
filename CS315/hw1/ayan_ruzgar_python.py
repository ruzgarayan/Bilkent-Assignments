import numpy as np


print("Question 1. What types are legal for subscripts?")
#Reference https://numpy.org/doc/stable/reference/arrays.indexing.html

arr = np.array([1, 2, 3, 10])
print(arr[True]) #[[1 2 3 10]]
print(arr[False]) #[]
print(arr[arr > 2]) #[3 10]
print(arr[[1,2]]) #[2 3]
print(arr[2, ...]) #[3]

#IndexError: only integers, slices (`:`), ellipsis (`...`), numpy.newaxis (`None`) and integer or boolean arrays are valid indices
#print(arr["abc"])

print("Question 2. Are subscripting expressions in element references range checked?")
print(arr[2]) #3
print(arr[-3]) # 2
#IndexError: index 4 is out of bounds for axis 0 with size 4
#print(arr[4])

print("Question 3. When are subscript ranges bound?")
#https://numpy.org/doc/stable/reference/generated/numpy.append.html
newarr = np.append(arr, 20)
print(newarr[4])

#Still IndexError on this one
#print(arr[4])


print("Question 5. Are ragged or rectangular multidimensional arrays allowed, or both?")

rectangular = np.array([[1,2,3],[4,5,6],[7,8,9]])
ragged = np.array([[1],[2,3],[4,5,6]], dtype=object)

#[[1 2 3] [4 5 6] [7 8 9]]
print("rectangular :", rectangular)
#[list([1]) list([2, 3]) list([4, 5, 6])]
print("ragged: ", ragged)

print("Question 6. Can array objects be initialized?")
initialized_arr = np.array([1, 2, 3, 10])
initialized_arr2 = np.full((2,3), "vegetable")
print(initialized_arr2)


print("Question 7. Are any kind of slices supported?")
print("arr ", arr) #[1, 2, 3, 10]
print("arr[1:3] ", arr[1:3]) #[2, 3]
print("arr[::2] ", arr[::2]) #[1, 3]
arr[::2] = [25, 25]
print("arr ", arr) #[25, 2, 25, 10]

print("Question 8. Which operators are provided?")
#Reference https://numpy.org/doc/stable/user/quickstart.html

print("[1, 2, 3, 4] + [4, 3, 2, 1] = ", np.array([1, 2, 3, 4]) + np.array([4,3,2,1])) #[5,5,5,5]
print("[1, 2, 3, 4] + 10 = ", np.array([1, 2, 3, 4]) + 10) #[11,12,13,14]
print("[1, 2, 3, 4] ** 2 = ", np.array([1, 2, 3, 4]) ** 2) #[1,4,9,16]

#Matrix product
print("[[1, 2], [3, 4]] * [[0, 1], [1, 0]] = ", np.array([[1, 2], [3, 4]]) @ np.array([[0, 1], [1, 0]]))
#[[2,1],[4,3]]