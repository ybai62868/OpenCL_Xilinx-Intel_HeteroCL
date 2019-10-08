import heterocl as hcl

hcl.init()

matrix_1_size = (10, 10)
matrix_2_size = (10, 10)
matrix_3_size = (matrix_1_size[0], matrix_2_size[1])

def gemm_compute(matrix_1, matrix_2):
	m = matrix_1.shape[0];
	n = matrix_2.shape[1];
	k = matrix_1.shape[1];
	assert matrix_1.shape[1] == matrix_2.shape[0]
	r = hcl.reduce_axis(0, k, 'k')
	temp = hcl.compute((m, n), lambda x, y: hcl.sum(matrix_1[x, r] * matrix_2[r, y], axis = r), name = 'matrix_3')
	return temp


matrix_1 = hcl.placeholder(matrix_1_size)
matrix_2 = hcl.placeholder(matrix_2_size)

s = hcl.create_schedule([matrix_1, matrix_2], gemm_compute)

code = hcl.build(s, target = 'sdaccel')
with open('default_function.cl', 'w') as f:
	f.write(code)



