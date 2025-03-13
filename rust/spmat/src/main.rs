use spmat::{CSR, matmat};

fn main() {
    let a = CSR::identity(3);
    // a.print();
    // let b = CSR::identity(3);
    let mut d = CSR::new(3, 3);
    d.insert(0, 0, 1.0);

    let c = matmat(&a, &d);

    // // let c = add(&a, &b);
    c.print_as_dense();
}
