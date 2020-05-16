
const routes = [
  {
    path: '/', 
    component: () => import('layouts/HomeLayout.vue'),
    children: [
      { 
        //path: '', component: () => import('pages/Index.vue'),
        path: 'domain/:domain', 
        component: () => import('pages/domain.vue')
      },
      {
        path: 'domain/:domain/module/:module', 
        component: () => import('pages/module.vue')
     },
     {
        path: 'domain/:domain/module/:module/section/:section', 
        component: () => import('pages/section.vue')
     }
    ]
  }
]

// Always leave this as last one
if (process.env.MODE !== 'ssr') {
  routes.push({
    path: '*',
    component: () => import('pages/Error404.vue')
  })
}

export default routes
