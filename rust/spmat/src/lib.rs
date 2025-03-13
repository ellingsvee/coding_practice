use std::fmt::Debug;
use std::process;

pub struct CSR {
    indices: Vec<usize>,
    indptr: Vec<usize>,
    data: Vec<f64>,
    nrows: usize,
    ncols: usize,
}

impl CSR {
    pub fn new(nrows: usize, ncols: usize) -> Self {
        let indptr = vec![0; nrows + 1];
        CSR {
            indices: Vec::new(),
            indptr,
            data: Vec::new(),
            nrows,
            ncols,
        }
    }

    pub fn insert(&mut self, row: usize, col: usize, value: f64) {
        assert!(row < self.nrows, "Row index out of bounds");
        assert!(col < self.ncols, "Column index out of bounds");

        let row_start = self.indptr[row];
        let row_end = *self.indptr.get(row + 1).unwrap_or(&self.indices.len());

        // Check if the entry already exists
        for i in row_start..row_end {
            if self.indices[i] == col {
                self.data[i] = value; // Overwrite existing value
                return;
            }
        }

        // Insert the new entry
        self.indices.insert(row_end, col);
        self.data.insert(row_end, value);

        // Update indptr for subsequent rows
        for r in (row + 1)..=self.nrows {
            self.indptr[r] += 1;
        }
    }

    pub fn print(&self) {
        println!("indices: {:?}", self.indices);
        println!("indptr: {:?}", self.indptr);
        println!("data: {:?}", self.data);
    }

    pub fn identity(n: usize) -> Self {
        let mut indptr = Vec::with_capacity(n + 1);
        let mut indices = Vec::with_capacity(n);
        let mut data = Vec::with_capacity(n);
        for i in 0..n {
            indptr.push(i);
            indices.push(i);
            data.push(1.0);
        }
        indptr.push(n);
        CSR {
            indices,
            indptr,
            data,
            nrows: n,
            ncols: n,
        }
    }

    pub fn print_as_dense(&self) {
        let mut i = 0;
        for row in 0..self.nrows {
            for col in 0..self.ncols {
                if i < self.indptr[row + 1] && self.indices[i] == col {
                    print!("{:?} ", self.data[i]);
                    i += 1;
                } else {
                    print!("{:?} ", f64::default());
                }
            }
            println!();
        }
    }
}

fn matmat_maxnnz(nrow: usize, ncol: usize, a: &CSR, b: &CSR) -> Result<usize, &'static str> {
    let mut mask = vec![-1; ncol];

    let mut nnz = 0;

    for i in 0..nrow {
        let mut row_nnz = 0;
        for jj in a.indptr[i]..a.indptr[i + 1] {
            let j = a.indices[jj];
            for kk in b.indptr[j]..b.indptr[j + 1] {
                let k = b.indices[kk];
                if mask[k] != i as isize {
                    mask[k] = i as isize;
                    row_nnz += 1;
                }
            }
        }

        let next_nnz = nnz + row_nnz;

        if row_nnz > usize::MAX - nnz {
            return Err("nnz of the result is too large");
        }
        nnz = next_nnz;
    }
    Ok(nnz)
}

pub fn matmat(a: &CSR, b: &CSR) -> CSR {
    let mut next: Vec<isize> = vec![-1; b.ncols];
    let mut sums: Vec<f64> = vec![0.0; b.ncols];

    let nrow: usize = a.nrows;
    let ncol: usize = b.ncols;

    // let nnz = 0;
    let max_nnz = matmat_maxnnz(nrow, ncol, a, b).unwrap_or_else(|err| {
        println!("Error when calculating max nnz: {}", err);
        process::exit(1);
    });
    // let max_nnz = matmat_maxnnz(nrow, ncol, a, b).unwrap();

    // Pre-allocate vectors with the estimated size
    let mut indptr = Vec::with_capacity(nrow + 1);
    let mut indices = Vec::with_capacity(max_nnz);
    let mut data = Vec::with_capacity(max_nnz);

    indptr.push(0);

    for i in 0..nrow {
        let mut head = -2;
        let mut length = 0;

        for jj in a.indptr[i]..a.indptr[i + 1] {
            let j = a.indices[jj];
            let v = a.data[jj];

            for kk in b.indptr[j]..b.indptr[j + 1] {
                let k = b.indices[kk];

                sums[k] += v * b.data[kk];

                if next[k] == -1 {
                    next[k] = head;
                    head = k as isize;
                    length += 1;
                }
            }
        }

        for _ in 0..length {
            if sums[head as usize] != 0.0 {
                indices.push(head as usize);
                data.push(sums[head as usize]);
            }

            let temp = head;
            head = next[head as usize];
            next[temp as usize] = -1;
            sums[temp as usize] = 0.0;
        }

        indptr.push(indices.len());
    }

    let c = CSR {
        indices,
        indptr,
        data,
        nrows: nrow,
        ncols: ncol,
    };

    return c;
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_csr_identity() {
        let csr = CSR::identity(3);
        assert_eq!(csr.nrows, 3);
        assert_eq!(csr.ncols, 3);
        assert_eq!(csr.indptr, vec![0, 1, 2, 3]);
        assert_eq!(csr.indices, vec![0, 1, 2]);
        assert_eq!(csr.data, vec![1.0, 1.0, 1.0]);
    }

    #[test]
    fn test_csr_insert() {
        let mut csr = CSR::new(3, 3);
        csr.insert(0, 0, 1.0);
        csr.insert(0, 1, 2.0);
        csr.insert(1, 1, 3.0);
        csr.insert(2, 2, 4.0);
        assert_eq!(csr.indptr, vec![0, 2, 3, 4]);
        assert_eq!(csr.indices, vec![0, 1, 1, 2]);
        assert_eq!(csr.data, vec![1.0, 2.0, 3.0, 4.0]);
    }
}
