"""Draw editable SVG UML and conceptual dispatch diagrams, task by task."""
from pathlib import Path
from html import escape
ROOT=Path(__file__).resolve().parents[1]

def draw(task,name,width,height,nodes,edges,notes):
    out=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
         '<rect width="100%" height="100%" fill="white"/>',
         '<defs><marker id="inherit" markerWidth="18" markerHeight="16" refX="17" refY="8" orient="auto"><path d="M1,1 L17,8 L1,15 Z" fill="white" stroke="#26384a" stroke-width="1.5"/></marker><marker id="call" markerWidth="12" markerHeight="12" refX="10" refY="6" orient="auto"><path d="M1,1 L10,6 L1,11" fill="none" stroke="#26384a" stroke-width="2"/></marker></defs>']
    shapes={}
    for title,x,y,w,attrs,methods in nodes:
        h=82+max(1,len(attrs))*24+max(1,len(methods))*24
        assert x>=0 and y>=0 and x+w<=width and y+h<=height
        shapes[title]=(x,y,w,h,attrs,methods)
    def point(title,side):
        x,y,w,h,*_=shapes[title]
        return {'top':(x+w/2,y),'bottom':(x+w/2,y+h),'left':(x,y+h/2),'right':(x+w,y+h/2)}[side]
    for child,parent,kind,sides,bends in edges:
        pts=[point(child,sides[0])]+bends+[point(parent,sides[1])]
        out.append('<polyline points="'+' '.join(f'{x},{y}' for x,y in pts)+f'" fill="none" stroke="#26384a" stroke-width="2" marker-end="url(#{kind})"/>')
    for title,(x,y,w,h,attrs,methods) in shapes.items():
        out.extend([f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="3" fill="#f5f8fb" stroke="#26384a" stroke-width="2"/>',
                    f'<text x="{x+w/2}" y="{y+29}" text-anchor="middle" font-family="Arial" font-size="20" font-weight="bold">{escape(title)}</text>',
                    f'<path d="M{x},{y+44} H{x+w}" stroke="#26384a"/>'])
        row=y+69
        for line in attrs or ['(fields omitted)']:
            out.append(f'<text x="{x+12}" y="{row}" font-family="Arial" font-size="17">{escape(line)}</text>');row+=24
        out.append(f'<path d="M{x},{row-8} H{x+w}" stroke="#26384a"/>');row+=15
        for line in methods or ['(operations omitted)']:
            out.append(f'<text x="{x+12}" y="{row}" font-family="Arial" font-size="17">{escape(line)}</text>');row+=24
    for x,y,line in notes:
        out.append(f'<text x="{x}" y="{y}" font-family="Arial" font-size="18" fill="#26384a">{escape(line)}</text>')
    out.append('</svg>')
    dest=ROOT/task/'diagrams'/name;dest.parent.mkdir(parents=True,exist_ok=True)
    dest.write_text('\n'.join(out)+'\n',encoding='utf-8',newline='\n')

draw('PartA/TaskA1','hierarchy.svg',1680,800,[
 ('Bill «abstract»',510,30,610,['- id_: int; - patient_: Patient (owned value)','- packageCode_: string'],['+ virtual total() const noexcept: double = 0','+ virtual type() const noexcept: const char* = 0','+ print(stream) const  {nonvirtual wrapper}','+ virtual ~Bill() noexcept'],),
 ('OutPatientBill',30,420,360,['- consultation_: double','- tests_: double'],['+ total()/type(): override','total = consultation + tests']),
 ('InPatientBill',430,420,380,['- days_: int; - perDay_: double','- procedures_: double','- medicines_: double'],['+ total()/type(): override','total = days × perDay','        + procedures + medicines']),
 ('DayCareBill',850,420,350,['- packagePrice_: double'],['+ total()/type(): override','total = packagePrice']),
 ('EmergencyBill',1240,420,400,['- base_: double; - hours_: double','- hourlyRate_: double'],['+ total()/type(): override','total = base + hours × hourlyRate']),
],[(x,'Bill «abstract»','inherit',('top','bottom'),[(cx,355),(815,355)]) for x,cx in [('OutPatientBill',210),('InPatientBill',620),('DayCareBill',1025),('EmergencyBill',1440)]],[(35,750,'Hollow triangle points to the public base. Pure virtual operations use = 0; concrete operations override them.'),(35,780,'Patient is held by value; Ledger owns unique_ptr<Bill>; borrowed Bill* arrays provide mixed-type processing.')])

draw('PartA/TaskA3','hierarchy.svg',1640,760,[
 ('Payer «abstract»',500,30,640,[],['+ virtual amountCovered(const Bill&) const: double = 0','+ virtual name() const noexcept: const char* = 0','+ virtual ~Payer() noexcept']),
 ('SelfPay',30,390,320,[],['+ amountCovered(): override','returns 0']),
 ('Insurance',390,390,380,['- limit_: double','- coPayment_: percent'],['+ amountCovered(): override','reject share above limit']),
 ('GovernmentScheme',810,390,390,['- packages_: map<code, amount>'],['+ amountCovered(): override','min(bill total, allowance)','unknown package throws']),
 ('CorporatePayer',1240,390,370,['- percent_: double','- cap_: double'],['+ amountCovered(): override','min(total × percent / 100, cap)','B4: added in separate header']),
],[(x,'Payer «abstract»','inherit',('top','bottom'),[(cx,330),(820,330)]) for x,cx in [('SelfPay',190),('Insurance',580),('GovernmentScheme',1005),('CorporatePayer',1425)]],[(30,715,'Bill is passed by const reference. The existing payer classes require no new type checks when CorporatePayer is added.'),(30,745,'Government map lookup is O(log p) for p packages; fixed-size sample policy invocation is bounded.')])

draw('PartA/TaskA4','exception_hierarchy.svg',1660,1130,[
 ('std::exception',620,20,400,[],['+ what() const noexcept']),
 ('std::runtime_error',40,280,420,[],['message-bearing runtime failures']),
 ('std::logic_error',650,280,400,[],['standard validation categories']),
 ('std::bad_alloc',1190,280,420,[],['allocation failure; injected test labelled']),
 ('BillingError',40,530,420,[],['hospital domain failure']),
 ('std::invalid_argument',560,530,450,[],['invalid stod input / negative TODO input']),
 ('std::out_of_range',1090,530,450,[],['stod numeric range failure']),
 ('ValidationError',30,780,360,[],['invalid charges or patient data']),
 ('ClaimError',480,780,350,[],['claim share exceeds cover']),
 ('PackageError',930,780,360,[],['unknown government package']),
 ('MissingPatientError',30,990,360,[],[]),
],[('std::runtime_error','std::exception','inherit',('top','bottom'),[(250,235),(820,235)]),('std::logic_error','std::exception','inherit',('top','bottom'),[(850,235),(820,235)]),('std::bad_alloc','std::exception','inherit',('top','bottom'),[(1400,235),(820,235)]),('BillingError','std::runtime_error','inherit',('top','bottom'),[]),('std::invalid_argument','std::logic_error','inherit',('top','bottom'),[(785,485),(850,485)]),('std::out_of_range','std::logic_error','inherit',('top','bottom'),[(1315,485),(850,485)]),('ValidationError','BillingError','inherit',('top','bottom'),[(210,735),(250,735)]),('ClaimError','BillingError','inherit',('top','bottom'),[(655,735),(250,735)]),('PackageError','BillingError','inherit',('top','bottom'),[(1110,735),(250,735)]),('MissingPatientError','ValidationError','inherit',('top','bottom'),[])],[(450,1060,'Catch specific errors before BillingError / std::exception.'),(450,1090,'Standard exception branches remain distinct; destructors do not throw.')])

draw('PartA/TaskA2','conceptual_vtables.svg',1680,1000,[
 ('Bill interface (abstract)',550,25,600,[],['total → pure virtual entry','type → pure virtual entry','destruction → virtual destructor role']),
 ('b1: Bill*',25,330,280,['points to derived object'],[]),
 ('OutPatientBill object',390,330,420,['conceptual vptr → table','patient / ID / OP charge fields'],[]),
 ('OutPatientBill table',1010,330,610,[],['total → OutPatientBill::total()','type → OutPatientBill::type()','destruction → OutPatientBill then Bill']),
 ('b2: Bill*',25,650,280,['points to derived object'],[]),
 ('InPatientBill object',390,650,420,['conceptual vptr → table','patient / ID / IP charge fields'],[]),
 ('InPatientBill table',1010,650,610,[],['total → InPatientBill::total()','type → InPatientBill::type()','destruction → InPatientBill then Bill']),
],[('b1: Bill*','OutPatientBill object','call',('right','left'),[]),('OutPatientBill object','OutPatientBill table','call',('right','left'),[]),('b2: Bill*','InPatientBill object','call',('right','left'),[]),('InPatientBill object','InPatientBill table','call',('right','left'),[])],[(25,925,'Common conceptual implementation only: C++ does not mandate a vptr, table order, slots or this layout.'),(25,955,'b->total() follows the dynamic object’s selected total entry. Abstract Bill cannot be a complete instantiated object.'),(25,985,'print()/show() is a nonvirtual wrapper that calls virtual operations; it needs no virtual slot of its own.')])
print('Created four task-local SVG diagrams')
