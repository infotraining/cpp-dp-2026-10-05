# **AI_DEVELOPMENT_MEMO.md**

## **Purpose**
This memo defines mandatory guidelines for AI‑assisted code generation within this C++ project. All AI tools must produce code that is safe, modern, maintainable, and consistent with the project’s architectural standards.


## **1. Language Standard**
All generated code must use **modern C++ (C++20 or C++23)**:

- RAII for all resource management  
- `std::unique_ptr`, `std::shared_ptr`, `std::optional`, `std::expected`, `std::span`  
- Range algorithms (`std::ranges`)  
- `constexpr` and `consteval` where appropriate  
- No raw `new` / `delete`  
- No owning raw pointers  
- No C‑style arrays  
- No C‑style casts  
- No macros except platform guards  


## **2. SOLID Principles**

### **Single Responsibility**
Each class or function must have one clear responsibility.

### **Open/Closed**
Extend behavior via new types or policies, not by modifying existing logic.

### **Liskov Substitution**
Interfaces must be substitutable without breaking invariants.

### **Interface Segregation**
Prefer small, focused interfaces over large, multi‑purpose ones.

### **Dependency Inversion**
High‑level modules depend on abstractions, not concrete implementations.


## **3. Error Handling Model**
Generated code must follow the project’s unified error model:

- Use `std::expected<T, Error>` for recoverable errors  
- Use exceptions only where explicitly allowed  
- No silent failures  
- No magic return values  
- No mixing of error‑handling styles within a module  


## **4. Memory and Lifetime Safety**
- RAII everywhere  
- No manual memory management  
- No global mutable state  
- Prefer value semantics  
- Use `std::span` for non‑owning views  
- Avoid dangling references  
- Avoid shared mutable state  


## **5. Concurrency and Async**
Generated code must follow project concurrency rules:

- No ad‑hoc threads  
- No blocking calls inside coroutine contexts  
- Use project‑approved executors/reactors  
- Avoid data races  
- Prefer message passing or immutability  


## **6. Architectural Boundaries**
AI‑generated code must respect project architecture:

- No cross‑module includes unless explicitly allowed  
- No leaking internal types into public headers  
- No cyclic dependencies  
- No hidden coupling  
- Follow naming conventions for namespaces, files, and types  
- Keep headers lightweight and self‑contained  


## **7. Build System Requirements**
- CMake targets must remain modular  
- No global include directories  
- No implicit dependencies  
- No hardcoded paths  
- No compiler‑specific hacks unless guarded  



## **8. Documentation Requirements**
Generated documentation must be:

- Accurate  
- Minimal but clear  
- Consistent with project terminology  
- Free of hallucinated APIs  
- Include examples when helpful  



## **9. Testing Requirements**
All AI‑generated code must include:

- Unit tests for logic  
- Tests for error paths  
- Tests for invariants  
- Benchmarks for performance‑critical components  



## **10. Transparency**
Every AI‑assisted commit must include:

```
AI-assisted: code generated with AI and manually reviewed.
```


## **11. Acceptable AI Usage**

### **Allowed**
- Generating small, isolated components  
- Drafting unit tests  
- Suggesting refactorings aligned with SOLID  
- Producing RAII wrappers  
- Creating adapters, policies, or small utilities  

### **Not Allowed**
- Designing core architecture  
- Generating large subsystems  
- Creating networking stacks, reactors, or frameworks  
- Modifying the project’s error model  
- Introducing new concurrency primitives  